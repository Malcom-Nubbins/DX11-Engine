#include "RenderClass.h"
#include "../../ApplicationNew.h"

ID3D11DepthStencilState* RenderClass::m_DepthDisabled(nullptr);
ID3D11DepthStencilState* RenderClass::m_DepthEnabled(nullptr);

ID3D11RasterizerState* RenderClass::m_NoCull(nullptr);
ID3D11RasterizerState* RenderClass::m_BackCull(nullptr);
ID3D11RasterizerState* RenderClass::m_Wireframe(nullptr);
ID3D11RasterizerState* RenderClass::m_Shadow(nullptr);

ID3D11BlendState* RenderClass::m_AlphaBlendState(nullptr);
ID3D11BlendState* RenderClass::m_AlphaBlendStateDisabled(nullptr);

bool RenderClass::m_DisableRTVClearing(false);

RenderClass::RenderClass()
{
	Initialise();
}

RenderClass::~RenderClass()
{
}

void RenderClass::Cleanup()
{
	m_DepthDisabled->Release();
	m_DepthDisabled = nullptr;

	m_DepthEnabled->Release();
	m_DepthEnabled = nullptr;

	if (m_NoCull)
	{
		m_NoCull->Release();
		m_NoCull = nullptr;
	}

	if (m_BackCull)
	{
		m_BackCull->Release();
		m_BackCull = nullptr;
	}

	if (m_Wireframe)
	{
		m_Wireframe->Release();
		m_Wireframe = nullptr;
	}

	if (m_Shadow)
	{
		m_Shadow->Release();
		m_Shadow = nullptr;
	}

	if(m_AlphaBlendState)
	{
		m_AlphaBlendState->Release();
		m_AlphaBlendState = nullptr;
	}

	if (m_AlphaBlendStateDisabled)
	{
		m_AlphaBlendStateDisabled->Release();
		m_AlphaBlendStateDisabled = nullptr;
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

	device->CreateDepthStencilState(&depthStencilDesc, &m_DepthDisabled);


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

	device->CreateDepthStencilState(&depthStencilDesc, &m_DepthEnabled);

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

	device->CreateBlendState(&blendDesc, &m_AlphaBlendState);

	blendDesc.RenderTarget[0].BlendEnable = false;

	device->CreateBlendState(&blendDesc, &m_AlphaBlendStateDisabled);

}

void RenderClass::ResizeViews()
{
}

void RenderClass::DisableZBuffer()
{
	ApplicationNew::Get().GetContext()->OMSetDepthStencilState(m_DepthDisabled, 0);
}

void RenderClass::EnableZBuffer()
{
	ApplicationNew::Get().GetContext()->OMSetDepthStencilState(m_DepthEnabled, 0);
}

void RenderClass::EnableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	ApplicationNew::Get().GetContext()->OMSetBlendState(m_AlphaBlendState, blendFactor, 0xffffffff);
}

void RenderClass::DisableAlphaBlending()
{
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	ApplicationNew::Get().GetContext()->OMSetBlendState(m_AlphaBlendStateDisabled, blendFactor, 0xffffffff);
}

void RenderClass::EnableRtvClearing()
{
	m_DisableRTVClearing = false;
}

void RenderClass::DisableRtvClearing()
{
	m_DisableRTVClearing = true;
}

void RenderClass::SetRasterizerState(RasterizerType rasterizer)
{
	auto device = ApplicationNew::Get().GetDevice();
	auto context = ApplicationNew::Get().GetContext();

	switch (rasterizer)
	{
	case RasterizerType::NO_CULL:
		if (m_NoCull == nullptr)
		{
			D3D11_RASTERIZER_DESC rasteriserdesc;
			ZeroMemory(&rasteriserdesc, sizeof(D3D11_RASTERIZER_DESC));
			rasteriserdesc.FillMode = D3D11_FILL_SOLID;
			rasteriserdesc.CullMode = D3D11_CULL_NONE;
			rasteriserdesc.MultisampleEnable = true;
			rasteriserdesc.AntialiasedLineEnable = false;
			rasteriserdesc.FrontCounterClockwise = false;
			device->CreateRasterizerState(&rasteriserdesc, &m_NoCull);
		}
		context->RSSetState(m_NoCull);
		break;

	case RasterizerType::BACK_CULL:
		if (m_BackCull == nullptr)
		{
			D3D11_RASTERIZER_DESC rasteriserdesc;
			ZeroMemory(&rasteriserdesc, sizeof(D3D11_RASTERIZER_DESC));
			rasteriserdesc.FillMode = D3D11_FILL_SOLID;
			rasteriserdesc.CullMode = D3D11_CULL_BACK;
			rasteriserdesc.MultisampleEnable = true;
			rasteriserdesc.AntialiasedLineEnable = true;
			rasteriserdesc.FrontCounterClockwise = false;
			device->CreateRasterizerState(&rasteriserdesc, &m_BackCull);
		}
		context->RSSetState(m_BackCull);
		break;

	case RasterizerType::WIREFRAME:
		if (m_Wireframe == nullptr)
		{
			D3D11_RASTERIZER_DESC wfdesc;
			ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
			wfdesc.FillMode = D3D11_FILL_WIREFRAME;
			wfdesc.CullMode = D3D11_CULL_NONE;
			device->CreateRasterizerState(&wfdesc, &m_Wireframe);
		}
		context->RSSetState(m_Wireframe);
		break;

	case RasterizerType::SHADOWDEPTH:
		if (m_Shadow == nullptr)
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
			device->CreateRasterizerState(&ddesc, &m_Shadow);
		}
		context->RSSetState(m_Shadow);
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

	if (renderTarget != nullptr && m_DisableRTVClearing == false)
		context->ClearRenderTargetView(renderTarget, clearColour);

	if (depthStencilView != nullptr)
		context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
