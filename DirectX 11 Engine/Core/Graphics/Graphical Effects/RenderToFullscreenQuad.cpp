#include "RenderToFullscreenQuad.h"
#include "../../ApplicationNew.h"

RenderToFullscreenQuad::RenderToFullscreenQuad()
	: _quadVS(nullptr)
		, _quadPS(nullptr)
		, _inputLayout(nullptr)
		, _quad()
		, m_MSAARTV(nullptr)
		, m_MSAARenderTargetTex2D(nullptr)
{
}

RenderToFullscreenQuad::~RenderToFullscreenQuad()
{
	_quad.vertexBuffer.Reset();
	_quad.indexBuffer.Reset();
}

void RenderToFullscreenQuad::Cleanup()
{
	m_MSAARenderTargetTex2D->Release();
	m_MSAARTV->Release();
	_inputLayout->Release();
	_quadVS->Release();
	_quadPS->Release();
}

HRESULT RenderToFullscreenQuad::Initialise(const float width, const float height)
{
	HRESULT hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Fullscreen Quad Shader", L"Error", MB_OK);
		return hr;
	}

	BuildQuad();

	InitRenderTargets(width, height);

	return S_OK;
}

HRESULT RenderToFullscreenQuad::InitRenderTargets(float width, float height)
{
	auto device = ApplicationNew::Get().GetDevice();

	D3D11_TEXTURE2D_DESC msaaTargetTexDesc;
	msaaTargetTexDesc.Width = static_cast<UINT>(width);
	msaaTargetTexDesc.Height = static_cast<UINT>(height);
	msaaTargetTexDesc.MipLevels = 1;
	msaaTargetTexDesc.ArraySize = 1;
	msaaTargetTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msaaTargetTexDesc.SampleDesc.Count = 4;
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
	ShaderClass::SetShadersAndInputLayout(_quadVS, nullptr, _inputLayout);
}

void RenderToFullscreenQuad::SetAsCurrentPixelShader() const
{
	ShaderClass::SetShadersAndInputLayout(nullptr, _quadPS, _inputLayout);
}

HRESULT RenderToFullscreenQuad::InitialiseShaders()
{
	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	
	HRESULT hr = ShaderClass::CreateVertexShader((WCHAR*)L"Core/Shaders/FullscreenQuadVS.cso", &_quadVS, &_inputLayout, quadLayout, ARRAYSIZE(quadLayout));
	if (FAILED(hr))
		return hr;
	hr = ShaderClass::CreatePixelShader((WCHAR*)L"Core/Shaders/FullscreenQuadPS.cso", &_quadPS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void RenderToFullscreenQuad::BuildQuad()
{
	HRESULT hr = BufferClass::CreateQuad(&_quad.vertexBuffer, &_quad.indexBuffer);
	if (FAILED(hr))
	{
		return;
	}

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	char const vbuffername[] = "Quad VB";
	char const ibuffername[] = "Quad IB";

	_quad.vertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(vbuffername) - 1, vbuffername);
	_quad.indexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(ibuffername) - 1, ibuffername);
#endif

	_quad.numberOfIndices = 6;
	_quad.vertexBufferOffset = 0;
	_quad.vertexBufferStride = sizeof(SimpleQuad);
}

void RenderToFullscreenQuad::Render(ID3D11ShaderResourceView * textureToRender) const
{
	auto context = ApplicationNew::Get().GetContext();

	RenderClass::SetRasterizerState(NO_CULL);
	context->PSSetSamplers(0, 1, ShaderClass::GetSamplerState(LINEAR));
	SetAsCurrentRenderTarget();
	SetAsCurrentVertexShader();
	SetAsCurrentPixelShader();
	
	context->IASetVertexBuffers(0, 1, _quad.vertexBuffer.GetAddressOf(), &_quad.vertexBufferStride, &_quad.vertexBufferOffset);
	context->IASetIndexBuffer(_quad.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->PSSetShaderResources(0, 1, &textureToRender);
	context->DrawIndexed(static_cast<UINT>(_quad.numberOfIndices), 0, 0);

	auto backBuff = ApplicationNew::Get().GetWindowByName(L"DX11 Engine")->GetBackBufferTex();

	context->ResolveSubresource(backBuff, 0, m_MSAARenderTargetTex2D, 0, DXGI_FORMAT_R8G8B8A8_UNORM);
}
