#include "HeatHaze.h"
#include "../../ApplicationNew.h"
#include "../../DX11Engine.h"
#include "../Core/Loaders/ConfigLoader.h"

HeatHaze::HeatHaze()
	: m_HeatHazePS(nullptr)
	, m_InputLayout(nullptr)
	, m_RenderTargetTex2D(nullptr)
	, m_RenderTargetView(nullptr)
	, m_RenderTargetSRV(nullptr)
	, m_HeatHazeValuesBufferPtr(nullptr)
	, m_SnowTex(nullptr)
	, m_HeatTex(nullptr)
	, m_HeatHazeValues()
{
}

HeatHaze::~HeatHaze()
{
}

void HeatHaze::Cleanup() const
{
	m_HeatHazePS->Release();
	//_inputLayout->Release();

	m_RenderTargetTex2D->Release();
	m_RenderTargetView->Release();
	m_RenderTargetSRV->Release();

	if (m_SnowTex)
	{
		m_SnowTex->Release();
	}
	
	if (m_HeatTex)
	{
		m_HeatTex->Release();
	}

	m_HeatHazeValuesBufferPtr->Release();
}

HRESULT HeatHaze::Initialise(const float width, const float height)
{
	HRESULT hr;
	hr = InitialiseRenderTarget(width, height);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Heat Haze Shader", L"Error", MB_OK);
		return hr;
	}

	hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Heat Haze RTV", L"Error", MB_OK);
		return hr;
	}

	BuildQuad();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(HeatHazeValues);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	ApplicationNew::Get().GetDevice()->CreateBuffer(&bd, nullptr, &m_HeatHazeValuesBufferPtr);

	const TextureHandler* texHandler = DX11Engine::Get().GetTextureHandler();
	m_SnowTex = texHandler->GetTextureByName("Snow");
	m_HeatTex = texHandler->GetTextureByName("DistortionMap");

	int msaaCount = ApplicationNew::Get().GetConfigLoader()->GetSettingValue(SettingType::Graphics, "MSAA");
	if (msaaCount == -1)
	{
		msaaCount = 0;
	}

	m_HeatHazeValues.SampleCount = static_cast<UINT>(msaaCount);

	return S_OK;
}

void HeatHaze::PreResize()
{
	m_RenderTargetTex2D->Release();
	m_RenderTargetView->Release();
	m_RenderTargetSRV->Release();
}

void HeatHaze::Resize(const float width, const float height)
{
	InitialiseRenderTarget(width, height);
}

HRESULT HeatHaze::InitialiseRenderTarget(const float width, const float height)
{
	auto device = ApplicationNew::Get().GetDevice();

	HRESULT hr;
	D3D11_TEXTURE2D_DESC renderTargetTexDesc;
	renderTargetTexDesc.Width = static_cast<UINT>(width);
	renderTargetTexDesc.Height = static_cast<UINT>(height);
	renderTargetTexDesc.MipLevels = 1;
	renderTargetTexDesc.ArraySize = 1;
	renderTargetTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetTexDesc.SampleDesc.Count = 4;
	renderTargetTexDesc.SampleDesc.Quality = 0;
	renderTargetTexDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTargetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTargetTexDesc.CPUAccessFlags = 0;
	renderTargetTexDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&renderTargetTexDesc, nullptr, &m_RenderTargetTex2D);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	renderTargetDesc.Format = renderTargetTexDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	renderTargetDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(m_RenderTargetTex2D, &renderTargetDesc, &m_RenderTargetView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create Render Target View (BasicLight)", L"Error", MB_OK);
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = renderTargetTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(m_RenderTargetTex2D, &srvDesc, &m_RenderTargetSRV);

	return S_OK;
}

HRESULT HeatHaze::InitialiseShaders()
{
	HRESULT hr = ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/HeatHazePS.cso", &m_HeatHazePS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void HeatHaze::BuildQuad()
{
	HRESULT hr = BufferClass::CreateQuad(&m_Quad.VertexBuffer, &m_Quad.IndexBuffer);
	if (FAILED(hr))
	{
		return;
	}

#if defined(_DEBUG)
	char const vbName[] = "Heat Haze VB";
	char const ibName[] = "Heat Haze IB";
	m_Quad.VertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(vbName) - 1, vbName);
	m_Quad.IndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(ibName) - 1, ibName);
#endif

	m_Quad.NumberOfIndices = 6;
	m_Quad.VertexBufferOffset = 0;
	m_Quad.vertexBufferStride = sizeof(SimpleQuad);
}

void HeatHaze::SetAsCurrentRenderTarget() const
{
	RenderClass::SetRenderTargetAndDepthStencil(m_RenderTargetView, nullptr);
}

void HeatHaze::SetAsCurrentPixelShader() const
{
	ShaderClass::SetShadersAndInputLayout(nullptr, m_HeatHazePS, nullptr);
}

void HeatHaze::Update(const float deltaTime)
{
	m_HeatHazeValues.Time = deltaTime;
}

void HeatHaze::Render(ID3D11ShaderResourceView * textureToProcess, std::string season)
{
	auto context = ApplicationNew::Get().GetContext();

	if (season == "Winter")
	{
		m_HeatHazeValues.Blizzard = 1.0f;
		m_HeatHazeValues.Heatwave = 0.0f;

		context->PSSetShaderResources(1, 1, &m_SnowTex);
	}
	else if (season == "Summer")
	{
		m_HeatHazeValues.Blizzard = 0.0f;
		m_HeatHazeValues.Heatwave = 1.0f;

		context->PSSetShaderResources(1, 1, &m_HeatTex);
	}

	BufferClass::SetPixelShaderBuffers(&m_HeatHazeValuesBufferPtr);

	context->PSSetSamplers(0, 1, ShaderClass::GetSamplerState(SamplerType::LINEAR));
	SetAsCurrentRenderTarget();
	SetAsCurrentPixelShader();

	context->IASetVertexBuffers(0, 1, m_Quad.VertexBuffer.GetAddressOf(), &m_Quad.vertexBufferStride, &m_Quad.VertexBufferOffset);
	context->IASetIndexBuffer(m_Quad.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->PSSetShaderResources(0, 1, &textureToProcess);

	context->UpdateSubresource(m_HeatHazeValuesBufferPtr, 0, nullptr, &m_HeatHazeValues, 0, 0);

	context->DrawIndexed(static_cast<UINT>(m_Quad.NumberOfIndices), 0, 0);
}
