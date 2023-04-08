#pragma once
#include "../../Globals/stdafx.h"

class RenderClass
{
private:
	static ID3D11DepthStencilState* m_DepthDisabled;
	static ID3D11DepthStencilState* m_DepthEnabled;

	static ID3D11RasterizerState* m_NoCull;
	static ID3D11RasterizerState* m_BackCull;
	static ID3D11RasterizerState* m_Wireframe;
	static ID3D11RasterizerState* m_Shadow;

	static ID3D11BlendState* m_AlphaBlendState;
	static ID3D11BlendState* m_AlphaBlendStateDisabled;

	static bool m_DisableRTVClearing;

public:
	RenderClass();
	~RenderClass();
	void Cleanup();

	void Initialise();
	void ResizeViews();

	static void DisableZBuffer();
	static void EnableZBuffer();

	static void EnableAlphaBlending();
	static void DisableAlphaBlending();

	static void EnableRtvClearing();
	static void DisableRtvClearing();

	static void SetRasterizerState(RasterizerType rasterizer);

	static void SetViewport(D3D11_VIEWPORT viewport);
	static void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView);
};

