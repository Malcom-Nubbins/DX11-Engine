#include "RenderClass.h"

RenderClass::RenderClass(D3DClass* d3dClass) : _d3dClass(d3dClass)
{

}

RenderClass::~RenderClass()
{
}

void RenderClass::Cleanup()
{

}

void RenderClass::ResizeViews()
{
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
