#include "Globals/stdafx.h"
#include "ApplicationNew.h"
#include "Loaders/ConfigLoader.h"
#include "DX11Engine.h"

#include <Shlwapi.h>
#include <dxgidebug.h>

std::map<std::string, UIOriginPoint> g_OriginEnumStrings;
std::map<std::string, UIAnchorPoint> g_AnchorEnumStrings;

void ReportLiveObjects()
{
#if defined(_DEBUG)
	IDXGIDebug1* dxgiDebug;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

	dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
	dxgiDebug->Release();
#endif
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
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
		C_ConfigLoader const* const configLoader = ApplicationNew::Get().GetConfigLoader();
		int const renderWidth = configLoader->GetSettingValue(SettingType::Video, "ScreenWidth");
		int const renderHeight = configLoader->GetSettingValue(SettingType::Video, "ScreenHeight");
		int const vsync = configLoader->GetSettingValue(SettingType::Video, "VSync");

		if (renderWidth == -1)
		{
			std::exception(R"(Invalid render width obtained from config file)");
			retCode = -1;
		}

		if (renderHeight == -1)
		{
			std::exception(R"(Invalid render height obtained from config file)");
			retCode = -1;
		}

		if (vsync == -1)
		{
			std::exception(R"(Invalid vsync value obtained from config file)");
		}

		if (retCode != -1)
		{
			std::shared_ptr<DX11Engine> dx11Engine = std::make_shared<DX11Engine>(L"DX11 Engine", static_cast<UINT>(renderWidth), static_cast<UINT>(renderHeight), static_cast<bool>(vsync));
			retCode = ApplicationNew::Get().Run(dx11Engine);
		}
	}

	ApplicationNew::Destroy();

	atexit(&ReportLiveObjects);

	return retCode;
}