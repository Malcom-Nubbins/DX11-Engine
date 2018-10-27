#include "RenderClass.h"

RenderClass::RenderClass(D3DClass* d3dClass) : _d3dClass(d3dClass)
{
	_depthDisabled = nullptr;
	_depthEnabled = nullptr;

	_noCull = nullptr;
	_backCull = nullptr;
	_wireframe = nullptr;
	_shadow = nullptr;
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

	//if (renderTarget != nullptr)
		//_d3dClass->GetContext()->ClearRenderTargetView(renderTarget, clearColour);

	if (depthStencilView != nullptr)
		_d3dClass->GetContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
