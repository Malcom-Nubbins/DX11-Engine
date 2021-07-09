#pragma once
#include "D3DClass.h"
#include "../../Globals/stdafx.h"
class RenderClass
{
private:
	static ID3D11DepthStencilState* _depthDisabled;
	static ID3D11DepthStencilState* _depthEnabled;

	static ID3D11RasterizerState* _noCull;
	static ID3D11RasterizerState* _backCull;
	static ID3D11RasterizerState* _wireframe;
	static ID3D11RasterizerState* _shadow;

	static ID3D11BlendState* _alphaBlendState;
	static ID3D11BlendState* _alphaBlendStateDisabled;

	static bool _disableRTVClearing;

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

	static void SetRasterizerState(RASTERIZER_TYPE rasterizer);

	static void SetViewport(D3D11_VIEWPORT viewport);
	static void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView);
};

