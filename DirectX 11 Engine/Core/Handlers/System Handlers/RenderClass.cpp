#include "RenderClass.h"
#include "../../ApplicationNew.h"

ID3D11DepthStencilState* RenderClass::_depthDisabled(nullptr);
ID3D11DepthStencilState* RenderClass::_depthEnabled(nullptr);

ID3D11RasterizerState* RenderClass::_noCull(nullptr);
ID3D11RasterizerState* RenderClass::_backCull(nullptr);
ID3D11RasterizerState* RenderClass::_wireframe(nullptr);
ID3D11RasterizerState* RenderClass::_shadow(nullptr);

ID3D11BlendState* RenderClass::_alphaBlendState(nullptr);
ID3D11BlendState* RenderClass::_alphaBlendStateDisabled(nullptr);

bool RenderClass::_disableRTVClearing(false);

RenderClass::RenderClass()
{
	Initialise();
}

RenderClass::~RenderClass()
{
}

void RenderClass::Cleanup()
{
	_depthDisabled->Release();
	_depthDisabled = nullptr;

	_depthEnabled->Release();
	_depthEnabled = nullptr;

	if (_noCull)
	{
		_noCull->Release();
		_noCull = nullptr;
	}

	if (_backCull)
	{
		_backCull->Release();
		_backCull = nullptr;
	}

	if (_wireframe)
	{
		_wireframe->Release();
		_wireframe = nullptr;
	}

	if (_shadow)
	{
		_shadow->Release();
		_shadow = nullptr;
	}

	if(_alphaBlendState)
	{
		_alphaBlendState->Release();
		_alphaBlendState = nullptr;
	}

	if (_alphaBlendStateDisabled)
	{
		_alphaBlendStateDisabled->Release();
		_alphaBlendStateDisabled = nullptr;
	}
}

void RenderClass::Initialise()
{
	auto device = ApplicationNew::Get().GetDevice();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&depthStencilDesc, &_depthDisabled);


	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&depthStencilDesc, &_depthEnabled);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	device->CreateBlendState(&blendDesc, &_alphaBlendState);

	blendDesc.RenderTarget[0].BlendEnable = false;

	device->CreateBlendState(&blendDesc, &_alphaBlendStateDisabled);

}

void RenderClass::ResizeViews()
{
}

void RenderClass::DisableZBuffer()
{
	ApplicationNew::Get().GetContext()->OMSetDepthStencilState(_depthDisabled, 0);
}

void RenderClass::EnableZBuffer()
{
	ApplicationNew::Get().GetContext()->OMSetDepthStencilState(_depthEnabled, 0);
}

void RenderClass::EnableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	ApplicationNew::Get().GetContext()->OMSetBlendState(_alphaBlendState, blendFactor, 0xffffffff);
}

void RenderClass::DisableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	ApplicationNew::Get().GetContext()->OMSetBlendState(_alphaBlendStateDisabled, blendFactor, 0xffffffff);
}

void RenderClass::EnableRtvClearing()
{
	_disableRTVClearing = false;
}

void RenderClass::DisableRtvClearing()
{
	_disableRTVClearing = true;
}

void RenderClass::SetRasterizerState(RASTERIZER_TYPE rasterizer)
{
	auto device = ApplicationNew::Get().GetDevice();
	auto context = ApplicationNew::Get().GetContext();

	switch (rasterizer)
	{
	case NO_CULL:
		if (_noCull == nullptr)
		{
			D3D11_RASTERIZER_DESC rasteriserdesc;
			ZeroMemory(&rasteriserdesc, sizeof(D3D11_RASTERIZER_DESC));
			rasteriserdesc.FillMode = D3D11_FILL_SOLID;
			rasteriserdesc.CullMode = D3D11_CULL_NONE;
			rasteriserdesc.MultisampleEnable = false;
			rasteriserdesc.AntialiasedLineEnable = false;
			rasteriserdesc.FrontCounterClockwise = false;
			device->CreateRasterizerState(&rasteriserdesc, &_noCull);
		}
		context->RSSetState(_noCull);
		break;

	case BACK_CULL:
		if (_backCull == nullptr)
		{
			D3D11_RASTERIZER_DESC rasteriserdesc;
			ZeroMemory(&rasteriserdesc, sizeof(D3D11_RASTERIZER_DESC));
			rasteriserdesc.FillMode = D3D11_FILL_SOLID;
			rasteriserdesc.CullMode = D3D11_CULL_BACK;
			rasteriserdesc.MultisampleEnable = true;
			rasteriserdesc.AntialiasedLineEnable = true;
			rasteriserdesc.FrontCounterClockwise = false;
			device->CreateRasterizerState(&rasteriserdesc, &_backCull);
		}
		context->RSSetState(_backCull);
		break;

	case WIREFRAME:
		if (_wireframe == nullptr)
		{
			D3D11_RASTERIZER_DESC wfdesc;
			ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
			wfdesc.FillMode = D3D11_FILL_WIREFRAME;
			wfdesc.CullMode = D3D11_CULL_NONE;
			device->CreateRasterizerState(&wfdesc, &_wireframe);
		}
		context->RSSetState(_wireframe);
		break;

	case SHADOWDEPTH:
		if (_shadow == nullptr)
		{
			D3D11_RASTERIZER_DESC ddesc;
			ZeroMemory(&ddesc, sizeof(D3D11_RASTERIZER_DESC));
			ddesc.FillMode = D3D11_FILL_SOLID;
			ddesc.CullMode = D3D11_CULL_NONE;
			ddesc.MultisampleEnable = false;
			ddesc.FrontCounterClockwise = false;
			ddesc.DepthBias = 5000;
			ddesc.DepthBiasClamp = 0.0f;
			ddesc.SlopeScaledDepthBias = 4.0f;
			device->CreateRasterizerState(&ddesc, &_shadow);
		}
		context->RSSetState(_shadow);
		break;

	default:

		break;
	}
}

void RenderClass::SetViewport(D3D11_VIEWPORT viewport)
{
	ApplicationNew::Get().GetContext()->RSSetViewports(1, &viewport);
}

void RenderClass::SetRenderTargetAndDepthStencil(ID3D11RenderTargetView * renderTarget, ID3D11DepthStencilView * depthStencilView)
{
	auto context = ApplicationNew::Get().GetContext();

	float clearColour[4] = { 0.0f, 0.0f, 0.0f, 100.0f };

	if (depthStencilView != nullptr && renderTarget != nullptr)
		context->OMSetRenderTargets(1, &renderTarget, depthStencilView);
	else if (depthStencilView != nullptr && renderTarget == nullptr)
		context->OMSetRenderTargets(0, nullptr, depthStencilView);
	else if (depthStencilView == nullptr && renderTarget != nullptr)
		context->OMSetRenderTargets(1, &renderTarget, nullptr);

	if (renderTarget != nullptr && _disableRTVClearing == false)
		context->ClearRenderTargetView(renderTarget, clearColour);

	if (depthStencilView != nullptr)
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
