#include "RenderToFullscreenQuad.h"

RenderToFullscreenQuad::RenderToFullscreenQuad(const SystemHandlers& systemHandlers)
	: _systemHandlers(systemHandlers), _backBuffer(nullptr), _quadVS(nullptr), _quadPS(nullptr), _inputLayout(nullptr),
	  _quad(),
	  _quadVertexBuffer(nullptr), _quadIndexBuffer(nullptr)
{
}


RenderToFullscreenQuad::~RenderToFullscreenQuad()
{
}

void RenderToFullscreenQuad::Cleanup()
{
	_backBuffer->Release();
	_backBuffer = nullptr;
}

HRESULT RenderToFullscreenQuad::Initialise(const float width, const float height)
{
	HRESULT hr = InitialiseBackBuffer(width, height);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Back Buffer", L"Error", MB_OK);
		return hr;
	}

	hr = InitialiseShaders();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to Create Fullscreen Quad Shader", L"Error", MB_OK);
		return hr;
	}

	BuildQuad();

	return S_OK;
}

void RenderToFullscreenQuad::Resize(const float width, const float height)
{
	_backBuffer->Release();
	_systemHandlers.GetD3DClass()->GetContext()->ClearState();

	
	HRESULT hr = _systemHandlers.GetD3DClass()->GetSwapChain()->ResizeBuffers(1, static_cast<UINT>(width), static_cast<UINT>(height), DXGI_FORMAT_UNKNOWN, 0);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"Failed to resize back buffer", L"Error", MB_OK);
	}
	
	InitialiseBackBuffer(width, height);
}

void RenderToFullscreenQuad::SetAsCurrentRenderTarget() const
{
	_systemHandlers.GetRenderClass()->SetRenderTargetAndDepthStencil(_backBuffer, nullptr);
}

void RenderToFullscreenQuad::SetAsCurrentVertexShader() const
{
	_systemHandlers.GetShaderClass()->SetShadersAndInputLayout(_quadVS, nullptr, _inputLayout);
}

void RenderToFullscreenQuad::SetAsCurrentPixelShader() const
{
	_systemHandlers.GetShaderClass()->SetShadersAndInputLayout(nullptr, _quadPS, _inputLayout);
}

HRESULT RenderToFullscreenQuad::InitialiseBackBuffer(float width, float height)
{
	HRESULT hr;
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _systemHandlers.GetD3DClass()->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));

	if (FAILED(hr))
		return hr;

	hr = _systemHandlers.GetD3DClass()->GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &_backBuffer);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT RenderToFullscreenQuad::InitialiseShaders()
{
	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	
	HRESULT hr = _systemHandlers.GetShaderClass()->CreateVertexShader((WCHAR*)L"Core/Shaders/FullscreenQuadVS.hlsl", &_quadVS, &_inputLayout, quadLayout, ARRAYSIZE(quadLayout));
	if (FAILED(hr))
		return hr;
	hr = _systemHandlers.GetShaderClass()->CreatePixelShader((WCHAR*)L"Core/Shaders/FullscreenQuadPS.hlsl", &_quadPS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void RenderToFullscreenQuad::BuildQuad()
{
	HRESULT hr = _systemHandlers.GetBufferClass()->CreateQuad(&_quad.vertexBuffer, &_quad.indexBuffer);
	if (FAILED(hr))
	{
		return;
	}

	_quad.numberOfIndices = 6;
	_quad.vertexBufferOffset = 0;
	_quad.vertexBufferStride = sizeof(SimpleQuad);
}

void RenderToFullscreenQuad::Render(ID3D11ShaderResourceView * textureToRender) const
{
	_systemHandlers.GetRenderClass()->SetRasterizerState(BACK_CULL);
	_systemHandlers.GetD3DClass()->GetContext()->PSSetSamplers(0, 1, _systemHandlers.GetShaderClass()->GetSamplerState(LINEAR));
	SetAsCurrentRenderTarget();
	SetAsCurrentVertexShader();
	SetAsCurrentPixelShader();
	
	_systemHandlers.GetD3DClass()->GetContext()->IASetVertexBuffers(0, 1, &_quad.vertexBuffer, &_quad.vertexBufferStride, &_quad.vertexBufferOffset);
	_systemHandlers.GetD3DClass()->GetContext()->IASetIndexBuffer(_quad.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	_systemHandlers.GetD3DClass()->GetContext()->PSSetShaderResources(0, 1, &textureToRender);
	_systemHandlers.GetD3DClass()->GetContext()->DrawIndexed(_quad.numberOfIndices, 0, 0);
}
