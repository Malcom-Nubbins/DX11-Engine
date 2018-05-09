#pragma once
#include "../Globals/stdafx.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
class D3DClass
{
private:
	D3D_DRIVER_TYPE	_driverType;
	D3D_FEATURE_LEVEL _featureLevel;
	ID3D11Device*	_device;
	ID3D11DeviceContext*	_deviceContext;
	IDXGISwapChain*	_swapChain;
public:
	D3DClass();
	~D3DClass();
	void Cleanup();

	HRESULT InitialiseDirectX(HWND& hwnd, float renderWidth, float renderHeight);

	ID3D11Device* GetDevice() const { return _device; }
	ID3D11DeviceContext* GetContext() const { return _deviceContext; }
	IDXGISwapChain* GetSwapChain() const { return _swapChain; }
};

