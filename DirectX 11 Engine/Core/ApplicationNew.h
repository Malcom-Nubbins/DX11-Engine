#pragma once
#include "Globals/stdafx.h"
#include "Globals/AppValues.h"

class WindowClass;
class EngineBase;
class C_ConfigLoader;

class ApplicationNew
{
public:
	static void Create(HINSTANCE hinst);
	static void Destroy();

	static ApplicationNew& Get();

	std::shared_ptr<WindowClass> CreateRenderWindow(std::wstring& windowName, UINT clientWidth, UINT clientHeight, bool vsync);

	void DestroyWindow(const std::wstring& windowName);
	void DestroyWindow(std::shared_ptr<WindowClass> window);

	std::shared_ptr<WindowClass> GetWindowByName(const std::wstring& windowName);

	int Run(std::shared_ptr<EngineBase> pEngineBase);
	void Quit(int exitCode = 0);

	ComPtr<ID3D11Device> const& GetDevice() const { return m_Device; }
	ComPtr<ID3D11DeviceContext> const& GetContext() const { return m_Context; }
#if defined(_DEBUG)
	ComPtr<ID3D11Debug> GetDebug() const { return m_Debug; }
#endif
	C_ConfigLoader const* GetConfigLoader() const { return m_ConfigLoader; }

	void ReloadConfigs();

protected:
	ApplicationNew(HINSTANCE hinst);
	virtual ~ApplicationNew();

	void Initialise();
	void Cleanup();

	ComPtr<IDXGIAdapter1> GetAdapter();
	void CreateDevice(ComPtr<IDXGIAdapter1> adapter);

private:
	ApplicationNew(const ApplicationNew& copy) = delete;
	ApplicationNew& operator=(const ApplicationNew& other) = delete;

	HINSTANCE m_hInstance;
	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_Context;
	ComPtr<ID3D11Debug> m_Debug;
	C_ConfigLoader* m_ConfigLoader;
};

