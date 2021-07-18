#pragma once
#include "../../Globals/stdafx.h"
#include "../../../resource.h"
#include "../../Globals/Events.h"
#include "../../Timer.h"

#include <string>
#include <memory>

class EngineBase;

class WindowClass
{
public:
	void Cleanup();

	HWND GetHWND() { return m_hWnd; }

	ID3D11Texture2D* GetBackBufferTex() const { return m_BackBufferTex2D.Get(); }
	ID3D11RenderTargetView* GetBackBuffer() const { return m_BackBuffer.Get(); }

	float GetWindowWidth() { return static_cast<float>(m_WindowWidth); }
	float GetWindowHeight() { return static_cast<float>(m_WindowHeight); }

	POINT GetScreenCentre() { return m_ScreenCentre; }

	const std::wstring& GetWindowName() const { return m_WindowCaption; }

	void Show();
	void Hide();

	bool IsVSync() const { return m_VSync; }
	void SetVsync(bool vsync) { m_VSync = vsync; }
	void ToggleVSync() { SetVsync(!m_VSync); }

	bool IsFullscreen() const { return m_Fullscreen; }
	void SetFullscreen(bool fullscreen);
	void ToggleFullscreen() { SetFullscreen(!m_Fullscreen); }

	UINT Present();

protected:
	friend LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	friend class ApplicationNew;
	friend class EngineBase;

	WindowClass() = delete;
	WindowClass(HWND& hwnd, std::wstring& windowName, UINT clientWidth, UINT clientHeight, bool vsync);
	virtual ~WindowClass();

	void RegisterCallbacks(std::shared_ptr<EngineBase> pEngineBase);

	virtual void OnUpdate(UpdateEvent& e);
	virtual void OnRender(RenderEvent& e);

	virtual void OnKeyPressed(KeyEvent& e);
	virtual void OnKeyReleased(KeyEvent& e);
	virtual void OnMouseMoved(MouseMotionEvent& e);
	virtual void OnMouseButtonDown(MouseButtonEvent& e);
	virtual void OnMouseButtonUp(MouseButtonEvent& e);
	virtual void OnMouseWheel(MouseWheelEvent& e);

	virtual void OnResize(UINT width, UINT height);

	void CreateSwapChain();

	void UpdateRenderTargetViews();

private:
	WindowClass(const WindowClass& copy) = delete;
	WindowClass& operator=(const WindowClass& other) = delete;

	HWND m_hWnd;

	std::wstring m_WindowCaption;

	UINT m_WindowWidth;
	UINT m_WindowHeight;
	bool m_VSync;
	bool m_Fullscreen;

	POINT m_ScreenCentre;
	RECT m_WindowRect;

	std::weak_ptr<EngineBase> m_EngineBase;

	ComPtr<IDXGISwapChain4> m_SwapChain;

	ComPtr<ID3D11Texture2D> m_BackBufferTex2D;
	ComPtr<ID3D11RenderTargetView> m_BackBuffer;

	uint64_t m_FrameCounter;
};

