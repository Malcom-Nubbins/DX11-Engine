#pragma once
#include "Globals/stdafx.h"

class WindowClass;
class EngineBase;

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

	ComPtr<ID3D11Device> GetDevice() const { return m_Device; }
	ComPtr<ID3D11DeviceContext> GetContext() const { return m_Context; }
	ComPtr<ID3D11Debug> GetDebug() const { return m_Debug; }

protected:
	ApplicationNew(HINSTANCE hinst);
	virtual ~ApplicationNew();

	void Initialise();

	ComPtr<IDXGIAdapter1> GetAdapter();
	ComPtr<ID3D11Device> CreateDevice(ComPtr<IDXGIAdapter1> adapter);

private:
	ApplicationNew(const ApplicationNew& copy) = delete;
	ApplicationNew& operator=(const ApplicationNew& other) = delete;

	HINSTANCE m_hInstance;
	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_Context;
	ID3D11Debug* m_Debug;
};

