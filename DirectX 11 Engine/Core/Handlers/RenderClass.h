#pragma once
#include "D3DClass.h"
#include "../Globals/stdafx.h"
class RenderClass
{
private:
	D3DClass * _d3dClass;
public:
	RenderClass(D3DClass* d3dClass);
	~RenderClass();
	void Cleanup();

	void ResizeViews();

	void SetViewport(D3D11_VIEWPORT viewport);
	void SetRenderTargetAndDepthStencil(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView);
};

