#pragma once
#include "../Globals/stdafx.h"
#include "../../resource.h"
class WindowClass
{
public:
public:
	WindowClass();
	~WindowClass();
	void Cleanup();

	HRESULT InitWindow(WNDCLASSEX& wcex, HINSTANCE hinst, int cmdShow, UINT windowWidth, UINT windowHeight);

	void SetWindowCaption(LPCWSTR newCaption);

	HWND GetHWND() { return _hWnd; }
	HINSTANCE GetHInstance() { return _hInstance; }

	void SetWindowResizing(bool state) { _resizing = state; }
	bool GetWindowResizing() { return _resizing; }

	void SetWindowMinimised(bool state) { _minimised = state; }
	void SetWindowMaximised(bool state) { _maximised = state; }

	bool GetWindowMinimised() { return _minimised; }
	bool GetWindowMaximised() { return _maximised; }

private:
	HWND _hWnd;
	HINSTANCE _hInstance;

	LPCWSTR _windowCaption;

	bool _minimised;
	bool _maximised;
	bool _resizing;
};

