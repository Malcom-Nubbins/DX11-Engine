#include "WindowClass.h"

WindowClass::WindowClass()
{
	_hWnd = nullptr;
	_hInstance = nullptr;
}

WindowClass::~WindowClass()
{
}

void WindowClass::Cleanup()
{
	_hInstance = nullptr;
	_hWnd = nullptr;
}

HRESULT WindowClass::InitWindow(WNDCLASSEX& wcex, HINSTANCE hinst, int cmdShow, UINT windowWidth, UINT windowHeight)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;

	_screenCentre.x = _windowWidth / 2;
	_screenCentre.y = _windowHeight / 2;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - _windowWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - _windowHeight) / 2;

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinst;
	wcex.hIcon = LoadIcon(hinst, (LPCTSTR)IDI_ICON1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"DX11EngineWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	SetWindowCaption(L"DX11 Engine");

	// Create window
	_hInstance = hinst;
	RECT rc = { 0, 0, _windowWidth, _windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"DX11EngineWindowClass", _windowCaption, WS_OVERLAPPEDWINDOW,
		posX, posY, _windowWidth, _windowHeight, nullptr, nullptr, _hInstance, nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, cmdShow);
	ShowCursor(false);

	return S_OK;
}

void WindowClass::SetWindowCaption(LPCWSTR newCaption)
{
	_windowCaption = newCaption;
	SetWindowText(_hWnd, _windowCaption);
}
