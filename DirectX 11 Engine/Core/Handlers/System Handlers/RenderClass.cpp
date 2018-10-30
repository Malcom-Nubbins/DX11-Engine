#include "RenderClass.h"

RenderClass::RenderClass(D3DClass* d3dClass) : _d3dClass(d3dClass)
{
	_depthDisabled = nullptr;
	_depthEnabled = nullptr;

	_noCull = nullptr;
	_backCull = nullptr;
	_wireframe = nullptr;
	_shadow = nullptr;

	_alphaBlendState = nullptr;
	_alphaBlendStateDisabled = nullptr;
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

	_d3dClass->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &_depthDisabled);


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

	_d3dClass->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &_depthEnabled);

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

	_d3dClass->GetDevice()->CreateBlendState(&blendDesc, &_alphaBlendState);

	blendDesc.RenderTarget[0].BlendEnable = false;

	_d3dClass->GetDevice()->CreateBlendState(&blendDesc, &_alphaBlendStateDisabled);

}

void RenderClass::ResizeViews()
{
}

void RenderClass::DisableZBuffer()
{
	_d3dClass->GetContext()->OMSetDepthStencilState(_depthDisabled, 0);
}

void RenderClass::EnableZBuffer()
{
	_d3dClass->GetContext()->OMSetDepthStencilState(_depthEnabled, 0);
}

void RenderClass::EnableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	_d3dClass->GetContext()->OMSetBlendState(_alphaBlendState, blendFactor, 0xffffffff);
}

void RenderClass::DisableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	_d3dClass->GetContext()->OMSetBlendState(_alphaBlendStateDisabled, blendFactor, 0xffffffff);
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
			_d3dClass->GetDevice()->CreateRasterizerState(&rasteriserdesc, &_noCull);
		}
		_d3dClass->GetContext()->RSSetState(_noCull);
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
			_d3dClass->GetDevice()->CreateRasterizerState(&rasteriserdesc, &_backCull);
		}
		_d3dClass->GetContext()->RSSetState(_backCull);
		break;

	case WIREFRAME:
		if (_wireframe == nullptr)
		{
			D3D11_RASTERIZER_DESC wfdesc;
			ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
			wfdesc.FillMode = D3D11_FILL_WIREFRAME;
			wfdesc.CullMode = D3D11_CULL_NONE;
			_d3dClass->GetDevice()->CreateRasterizerState(&wfdesc, &_wireframe);
		}
		_d3dClass->GetContext()->RSSetState(_wireframe);
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
			_d3dClass->GetDevice()->CreateRasterizerState(&ddesc, &_shadow);
		}
		_d3dClass->GetContext()->RSSetState(_shadow);
		break;

	default:

		break;
	}
}

void RenderClass::SetViewport(D3D11_VIEWPORT viewport)
{
	_d3dClass->GetContext()->RSSetViewports(1, &viewport);
}

void RenderClass::SetRenderTargetAndDepthStencil(ID3D11RenderTargetView * renderTarget, ID3D11DepthStencilView * depthStencilView)
{
	float clearColour[4] = { 0.0f, 0.0f, 0.0f, 100.0f };

	if (depthStencilView != nullptr && renderTarget != nullptr)
		_d3dClass->GetContext()->OMSetRenderTargets(1, &renderTarget, depthStencilView);
	else if (depthStencilView != nullptr && renderTarget == nullptr)
		_d3dClass->GetContext()->OMSetRenderTargets(0, nullptr, depthStencilView);
	else if (depthStencilView == nullptr && renderTarget != nullptr)
		_d3dClass->GetContext()->OMSetRenderTargets(1, &renderTarget, nullptr);

	if (renderTarget != nullptr && _disableRTVClearing == false)
		_d3dClass->GetContext()->ClearRenderTargetView(renderTarget, clearColour);

	if (depthStencilView != nullptr)
		_d3dClass->GetContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
