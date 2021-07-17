#include "RenderToFullscreenQuad.h"
#include "../../ApplicationNew.h"

RenderToFullscreenQuad::RenderToFullscreenQuad()
	: _quadVS(nullptr), _quadPS(nullptr), _inputLayout(nullptr),
	  _quad(),
	  _quadVertexBuffer(nullptr), _quadIndexBuffer(nullptr)
{
}


RenderToFullscreenQuad::~RenderToFullscreenQuad()
{
}

void RenderToFullscreenQuad::Cleanup()
{
	_inputLayout->Release();
	_quadVS->Release();
	_quadPS->Release();
	_quad.vertexBuffer->Release();
	_quad.indexBuffer->Release();
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

	return S_OK;
}

void RenderToFullscreenQuad::SetAsCurrentRenderTarget() const
{
	RenderClass::SetRenderTargetAndDepthStencil(ApplicationNew::Get().GetWindowByName(L"DX11 Engine")->GetBackBuffer().Get(), nullptr);
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
	
	context->IASetVertexBuffers(0, 1, &_quad.vertexBuffer, &_quad.vertexBufferStride, &_quad.vertexBufferOffset);
	context->IASetIndexBuffer(_quad.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->PSSetShaderResources(0, 1, &textureToRender);
	context->DrawIndexed(static_cast<UINT>(_quad.numberOfIndices), 0, 0);
}
