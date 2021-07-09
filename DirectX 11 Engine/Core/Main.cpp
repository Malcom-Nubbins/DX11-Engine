#include "Globals/stdafx.h"
#include "ApplicationNew.h"
#include "DX11Engine.h"

#include <Shlwapi.h>
#include <dxgidebug.h>

void ReportLiveObjects()
{
	IDXGIDebug1* dxgiDebug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

	dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
	dxgiDebug->Release();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	int retCode = 0;

	WCHAR path[MAX_PATH];
	HMODULE hModule = GetModuleHandleW(nullptr);
	if (GetModuleFileNameW(hModule, path, MAX_PATH) > 0)
	{
		PathRemoveFileSpecW(path);
		SetCurrentDirectoryW(path);
	}

	ApplicationNew::Create(hInstance);
	{
		std::shared_ptr<DX11Engine> dx11Engine = std::make_shared<DX11Engine>(L"DX11 Engine", 1280, 720, true);
		retCode = ApplicationNew::Get().Run(dx11Engine);
	}

	ApplicationNew::Destroy();

	atexit(&ReportLiveObjects);

	return retCode;
}