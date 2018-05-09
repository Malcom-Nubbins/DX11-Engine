#include "RenderToFullscreenQuad.h"

RenderToFullscreenQuad::RenderToFullscreenQuad(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass) 
	: _d3dClass(d3dClass), _shaderClass(shaderClass), _renderClass(renderClass), _bufferClass(bufferClass)
{
	_backBuffer = nullptr;
}


RenderToFullscreenQuad::~RenderToFullscreenQuad()
{
}

void RenderToFullscreenQuad::Cleanup()
{
	_backBuffer->Release();
	_backBuffer = nullptr;
}

HRESULT RenderToFullscreenQuad::Initialise(float width, float height)
{
	HRESULT hr;

	hr = InitialiseBackBuffer(width, height);
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

void RenderToFullscreenQuad::Resize(float width, float height)
{
	_backBuffer->Release();

	_d3dClass->GetSwapChain()->ResizeBuffers(1, static_cast<UINT>(width), static_cast<UINT>(height), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	InitialiseBackBuffer(width, height);
}

void RenderToFullscreenQuad::SetAsCurrentRenderTarget()
{
	_renderClass->SetRenderTargetAndDepthStencil(_backBuffer, nullptr);
}

void RenderToFullscreenQuad::SetAsCurrentVertexShader()
{
	_shaderClass->SetShadersAndInputLayout(_quadVS, nullptr, _inputLayout);
}

void RenderToFullscreenQuad::SetAsCurrentPixelShader()
{
	_shaderClass->SetShadersAndInputLayout(nullptr, _quadPS, _inputLayout);
}

HRESULT RenderToFullscreenQuad::InitialiseBackBuffer(float width, float height)
{
	HRESULT hr;
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _d3dClass->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _d3dClass->GetDevice()->CreateRenderTargetView(pBackBuffer, nullptr, &_backBuffer);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT RenderToFullscreenQuad::InitialiseShaders()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC quadLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _shaderClass->CreateVertexShader((WCHAR*)L"Core/Shaders/FullscreenQuadVS.hlsl", &_quadVS, &_inputLayout, quadLayout, ARRAYSIZE(quadLayout));
	if (FAILED(hr))
		return hr;
	hr = _shaderClass->CreatePixelShader((WCHAR*)L"Core/Shaders/FullscreenQuadPS.hlsl", &_quadPS);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void RenderToFullscreenQuad::BuildQuad()
{
	HRESULT hr;
	hr = _bufferClass->CreateQuad(&_quad.vertexBuffer, &_quad.indexBuffer);
	if (FAILED(hr))
	{
		return;
	}

	_quad.numberOfIndices = 6;
	_quad.vertexBufferOffset = 0;
	_quad.vertexBufferStride = sizeof(SimpleQuad);
}

void RenderToFullscreenQuad::Render(ID3D11ShaderResourceView * textureToRender)
{
	_d3dClass->GetContext()->PSSetSamplers(0, 1, _shaderClass->GetSamplerState(LINEAR));
	SetAsCurrentRenderTarget();
	SetAsCurrentVertexShader();
	SetAsCurrentPixelShader();
	
	_d3dClass->GetContext()->IASetVertexBuffers(0, 1, &_quad.vertexBuffer, &_quad.vertexBufferStride, &_quad.vertexBufferOffset);
	_d3dClass->GetContext()->IASetIndexBuffer(_quad.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	_d3dClass->GetContext()->PSSetShaderResources(0, 1, &textureToRender);
	_d3dClass->GetContext()->DrawIndexed(_quad.numberOfIndices, 0, 0);
}
