#include "HeatHaze.h"
#include "../../ApplicationNew.h"
#include "../../DX11Engine.h"

HeatHaze::HeatHaze()
	: _quad(), _values()
{
	_heatHazePS = nullptr;
	_inputLayout = nullptr;

	_renderTargetTex2D = nullptr;
	_renderTargetView = nullptr;
	_renderTargetSRV = nullptr;

	_heatHazeValues = nullptr;
}

HeatHaze::~HeatHaze()
{
}

void HeatHaze::Cleanup() const
{
	_heatHazePS->Release();
	//_inputLayout->Release();

	_renderTargetTex2D->Release();
	_renderTargetView->Release();
	_renderTargetSRV->Release();

	_heatHazeValues->Release();
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
	ApplicationNew::Get().GetDevice()->CreateBuffer(&bd, nullptr, &_heatHazeValues);

	return S_OK;
}

void HeatHaze::PreResize()
{
	_renderTargetTex2D->Release();
	_renderTargetView->Release();
	_renderTargetSRV->Release();
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

	hr = device->CreateTexture2D(&renderTargetTexDesc, nullptr, &_renderTargetTex2D);
	if (FAILED(hr))
	{
		return hr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	renderTargetDesc.Format = renderTargetTexDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	renderTargetDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(_renderTargetTex2D, &renderTargetDesc, &_renderTargetView);
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

	device->CreateShaderResourceView(_renderTargetTex2D, &srvDesc, &_renderTargetSRV);

	return S_OK;
}

HRESULT HeatHaze::InitialiseShaders()
{
	HRESULT hr = ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/HeatHazePS.cso", &_heatHazePS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void HeatHaze::BuildQuad()
{
	HRESULT hr = BufferClass::CreateQuad(&_quad.vertexBuffer, &_quad.indexBuffer);
	if (FAILED(hr))
	{
		return;
	}

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	char const vbName[] = "Heat Haze VB";
	char const ibName[] = "Heat Haze IB";
	_quad.vertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(vbName) - 1, vbName);
	_quad.indexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(ibName) - 1, ibName);
#endif

	_quad.numberOfIndices = 6;
	_quad.vertexBufferOffset = 0;
	_quad.vertexBufferStride = sizeof(SimpleQuad);
}

void HeatHaze::SetAsCurrentRenderTarget() const
{
	RenderClass::SetRenderTargetAndDepthStencil(_renderTargetView, nullptr);
}

void HeatHaze::SetAsCurrentPixelShader() const
{
	ShaderClass::SetShadersAndInputLayout(nullptr, _heatHazePS, nullptr);
}

void HeatHaze::Update(const float deltaTime)
{
	_values.time = deltaTime;
}

void HeatHaze::Render(ID3D11ShaderResourceView * textureToProcess, std::string season)
{
	auto context = ApplicationNew::Get().GetContext();
	auto texHandler = DX11Engine::Get().GetTextureHandler();

	if (season == "Winter")
	{
		_values.blizzard = 1.0f;
		_values.heatwave = 0.0f;

		ID3D11ShaderResourceView* snowTex = texHandler->GetTextureByName("Snow");
		context->PSSetShaderResources(1, 1, &snowTex);
	}
	else if (season == "Summer")
	{
		_values.blizzard = 0.0f;
		_values.heatwave = 1.0f;

		ID3D11ShaderResourceView* distortionMap = texHandler->GetTextureByName("DistortionMap");
		context->PSSetShaderResources(1, 1, &distortionMap);
	}

	BufferClass::SetPixelShaderBuffers(&_heatHazeValues);

	context->PSSetSamplers(0, 1, ShaderClass::GetSamplerState(LINEAR));
	SetAsCurrentRenderTarget();
	SetAsCurrentPixelShader();

	context->IASetVertexBuffers(0, 1, _quad.vertexBuffer.GetAddressOf(), &_quad.vertexBufferStride, &_quad.vertexBufferOffset);
	context->IASetIndexBuffer(_quad.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->PSSetShaderResources(0, 1, &textureToProcess);

	context->UpdateSubresource(_heatHazeValues, 0, nullptr, &_values, 0, 0);

	context->DrawIndexed(static_cast<UINT>(_quad.numberOfIndices), 0, 0);
}
