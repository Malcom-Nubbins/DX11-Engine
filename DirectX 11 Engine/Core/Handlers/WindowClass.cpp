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
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinst;
	wcex.hIcon = LoadIcon(hinst, (LPCTSTR)IDI_ICON1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"FYPWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	SetWindowCaption(L"FYP - Seasonal Day/Night Cycle");

	// Create window
	_hInstance = hinst;
	RECT rc = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"FYPWindowClass", _windowCaption, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, _hInstance,
		nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, cmdShow);

	return S_OK;
}

void WindowClass::SetWindowCaption(LPCWSTR newCaption)
{
	_windowCaption = newCaption;
	SetWindowText(_hWnd, _windowCaption);
}
