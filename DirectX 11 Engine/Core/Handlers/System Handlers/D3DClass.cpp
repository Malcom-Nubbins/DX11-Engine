#include "D3DClass.h"

D3DClass::D3DClass() : _device(nullptr), _deviceContext(nullptr), _swapChain(nullptr), _driverType(D3D_DRIVER_TYPE_NULL), _featureLevel(D3D_FEATURE_LEVEL_11_0)
{
}

D3DClass::~D3DClass()
{
}

void D3DClass::Cleanup()
{
	_deviceContext->ClearState();
	_deviceContext->Release();
	_swapChain->Release();
	_device->Release();

	_device = nullptr;
	_deviceContext = nullptr;
	_swapChain = nullptr;
}

HRESULT D3DClass::InitialiseDirectX(HWND& hwnd, float renderWidth, float renderHeight)
{
	HRESULT hr;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
		D3D_DRIVER_TYPE_WARP
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	DXGI_MODE_DESC* displayModeList;
	IDXGIOutput* adapterOutput;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;

	unsigned int numModes, i, numerator, denominator;

	numerator = 60;
	denominator = 1;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
		return hr;

	hr = factory->EnumAdapters(0, &adapter);
	if (FAILED(hr))
		return hr;

	hr = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(hr))
		return hr;

	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(hr))
		return hr;


	displayModeList = new DXGI_MODE_DESC[numModes];
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(hr))
		return hr;

	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)renderWidth)
		{
			if (displayModeList[i].Height == (unsigned int)renderHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);
	UINT sampleCount = 1;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = (UINT)renderWidth;
	sd.BufferDesc.Height = (UINT)renderHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = numerator;
	sd.BufferDesc.RefreshRate.Denominator = denominator;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = sampleCount;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, 0, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_swapChain, &_device, &_featureLevel, &_deviceContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	return hr;
}
