#pragma once
#include "D3DClass.h"
#include "../../Globals/stdafx.h"
class RenderClass
{
private:
	D3DClass * _d3dClass;
	ID3D11DepthStencilState* _depthDisabled;
	ID3D11DepthStencilState* _depthEnabled;

	ID3D11RasterizerState* _noCull;
	ID3D11RasterizerState* _backCull;
	ID3D11RasterizerState* _wireframe;
	ID3D11RasterizerState* _shadow;

	ID3D11BlendState* _alphaBlendState;
	ID3D11BlendState* _alphaBlendStateDisabled;

	bool _disableRTVClearing;

public:
	RenderClass(D3DClass* d3dClass);
	~RenderClass();
	void Cleanup();

	void Initialise();
	void ResizeViews();

	void DisableZBuffer();
	void EnableZBuffer();

	void EnableAlphaBlending();
	void DisableAlphaBlending();

	void EnableRtvClearing();
	void DisableRtvClearing();

	void SetRasterizerState(RASTERIZER_TYPE rasterizer);

	void SetViewport(D3D11_VIEWPORT viewport);
	void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView);
};

