#include "RenderToFullscreenQuad.h"
#include "../../ApplicationNew.h"
#include "../../Loaders/ConfigLoader.h"
#include "../Core/Handlers/System Handlers/BufferClass.h"
#include "../Core/Handlers/System Handlers/ShaderClass.h"
#include "../Core/Handlers/System Handlers/RenderClass.h"
#include "../Core/Handlers/System Handlers/WindowClass.h"

RenderToFullscreenQuad::RenderToFullscreenQuad()
	: m_QuadVS(nullptr)
	, m_QuadPS(nullptr)
	, m_InputLayout(nullptr)
	, m_MSAARTV(nullptr)
	, m_MSAARenderTargetTex2D(nullptr)
	, m_MSAACount(0)
	, m_ValuesBufferPtr(nullptr)
{
}

RenderToFullscreenQuad::~RenderToFullscreenQuad()
{
	m_Quad.VertexBuffer.Reset();
	m_Quad.IndexBuffer.Reset();
}

void RenderToFullscreenQuad::Cleanup()
{
	m_MSAARenderTargetTex2D->Release();
	m_MSAARTV->Release();
	m_InputLayout->Release();
	m_QuadVS->Release();
	m_QuadPS->Release();
	m_ValuesBufferPtr->Release();
}

HRESULT RenderToFullscreenQuad::Initialise(const float width, const float height)
{
	HRESULT hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Fullscreen Quad Shader", L"Error", MB_OK);
		return hr;
	}

	int msaaCount = ApplicationNew::Get().GetConfigLoader()->GetSettingValue(SettingType::Graphics, "MSAA");
	if (msaaCount == -1)
	{
		msaaCount = 0;
	}

	m_MSAACount = static_cast<UINT>(msaaCount);

	BuildQuad();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(FullscreenQuadValues);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	ApplicationNew::Get().GetDevice()->CreateBuffer(&bd, nullptr, &m_ValuesBufferPtr);

	InitRenderTargets(width, height);

	return S_OK;
}

HRESULT RenderToFullscreenQuad::InitRenderTargets(float width, float height)
{
	auto device = ApplicationNew::Get().GetDevice();

	UINT sampleQuality;
	device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, m_MSAACount, &sampleQuality);

	D3D11_TEXTURE2D_DESC msaaTargetTexDesc;
	msaaTargetTexDesc.Width = static_cast<UINT>(width);
	msaaTargetTexDesc.Height = static_cast<UINT>(height);
	msaaTargetTexDesc.MipLevels = 1;
	msaaTargetTexDesc.ArraySize = 1;
	msaaTargetTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msaaTargetTexDesc.SampleDesc.Count = m_MSAACount;
	msaaTargetTexDesc.SampleDesc.Quality = 0;
	msaaTargetTexDesc.Usage = D3D11_USAGE_DEFAULT;
	msaaTargetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	msaaTargetTexDesc.CPUAccessFlags = 0;
	msaaTargetTexDesc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&msaaTargetTexDesc, nullptr, &m_MSAARenderTargetTex2D);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create MSAA render target tex2d", L"Error", MB_OK);
		return hr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc{};
	renderTargetDesc.Format = msaaTargetTexDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	renderTargetDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(m_MSAARenderTargetTex2D, &renderTargetDesc, &m_MSAARTV);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to create MSAA render target", L"Error", MB_OK);
		return hr;
	}

	return S_OK;
}

void RenderToFullscreenQuad::PreResize()
{
	m_MSAARenderTargetTex2D->Release();
	m_MSAARTV->Release();
}

void RenderToFullscreenQuad::OnResize(float width, float height)
{
	InitRenderTargets(width, height);
}

void RenderToFullscreenQuad::SetAsCurrentRenderTarget() const
{
	RenderClass::SetRenderTargetAndDepthStencil(m_MSAARTV, nullptr);
	//RenderClass::SetRenderTargetAndDepthStencil(ApplicationNew::Get().GetWindowByName(L"DX11 Engine")->GetBackBuffer().Get(), nullptr);
}

void RenderToFullscreenQuad::SetAsCurrentVertexShader() const
{
	ShaderClass::SetShadersAndInputLayout(m_QuadVS, nullptr, m_InputLayout);
}

void RenderToFullscreenQuad::SetAsCurrentPixelShader() const
{
	ShaderClass::SetShadersAndInputLayout(nullptr, m_QuadPS, m_InputLayout);
}

HRESULT RenderToFullscreenQuad::InitialiseShaders()
{
	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	
	HRESULT hr = ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/FullscreenQuadVS.cso", &m_QuadVS, &m_InputLayout, quadLayout, ARRAYSIZE(quadLayout));
	if (FAILED(hr))
		return hr;
	hr = ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/FullscreenQuadPS.cso", &m_QuadPS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void RenderToFullscreenQuad::BuildQuad()
{
	HRESULT hr = BufferClass::CreateQuad(&m_Quad.VertexBuffer, &m_Quad.IndexBuffer);
	if (FAILED(hr))
	{
		return;
	}

#if defined(_DEBUG)
	char const vbuffername[] = "Quad VB";
	char const ibuffername[] = "Quad IB";

	m_Quad.VertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(vbuffername) - 1, vbuffername);
	m_Quad.IndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(ibuffername) - 1, ibuffername);
#endif

	m_Quad.NumberOfIndices = 6;
	m_Quad.VertexBufferOffset = 0;
	m_Quad.vertexBufferStride = sizeof(SimpleQuad);
}

void RenderToFullscreenQuad::Render(ID3D11ShaderResourceView * textureToRender)
{
	auto context = ApplicationNew::Get().GetContext();
	FullscreenQuadValues values;
	values.SampleCount = m_MSAACount;

	RenderClass::SetRasterizerState(RasterizerType::NO_CULL);
	context->PSSetSamplers(0, 1, ShaderClass::GetSamplerState(SamplerType::LINEAR));
	SetAsCurrentRenderTarget();
	SetAsCurrentVertexShader();
	SetAsCurrentPixelShader();
	BufferClass::SetPixelShaderBuffers(&m_ValuesBufferPtr, 0);
	
	context->IASetVertexBuffers(0, 1, m_Quad.VertexBuffer.GetAddressOf(), &m_Quad.vertexBufferStride, &m_Quad.VertexBufferOffset);
	context->IASetIndexBuffer(m_Quad.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->PSSetShaderResources(0, 1, &textureToRender);

	context->UpdateSubresource(m_ValuesBufferPtr, 0, nullptr, &values, 0, 0);

	context->DrawIndexed(static_cast<UINT>(m_Quad.NumberOfIndices), 0, 0);

	ID3D11Texture2D* backBuff = ApplicationNew::Get().GetWindowByName(L"DX11 Engine")->GetBackBufferTex();

	context->ResolveSubresource(backBuff, 0, m_MSAARenderTargetTex2D, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
}
