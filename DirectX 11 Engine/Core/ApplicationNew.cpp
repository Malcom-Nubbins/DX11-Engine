#include "ApplicationNew.h"

#include "Handlers/System Handlers/WindowClass.h"
#include "Handlers/System Handlers/InputHandler.h"
#include "EngineBase.h"
#include "Loaders/ConfigLoader.h"
#include "Globals/AppValues.h"

#include <map>

const wchar_t WINDOW_CLASS_NAME[] = L"DX11RenderWindowClass";

using WindowPtr = std::shared_ptr<WindowClass>;
using WindowMap = std::map<HWND, WindowPtr>;
using WindowNameMap = std::map<std::wstring, WindowPtr>;

static ApplicationNew* g_App = nullptr;
static WindowMap g_Windows;
static WindowNameMap g_WindowByName;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

struct MakeWindow : public WindowClass
{
	MakeWindow(HWND& hWnd, std::wstring& windowName, UINT clientWidth, UINT clientHeight, bool vsync)
		: WindowClass(hWnd, windowName, clientWidth, clientHeight, vsync)
	{}
};

void ApplicationNew::Create(HINSTANCE hinst)
{
	if (!g_App)
	{
		g_App = new ApplicationNew(hinst);
		g_App->Initialise();
	}
}

void ApplicationNew::Destroy()
{
	if (g_App)
	{
		g_App->Cleanup();

		assert(g_Windows.empty() && g_WindowByName.empty() && "All windows must be destroyed first");

		delete g_App;
		g_App = nullptr;
	}
}

ApplicationNew& ApplicationNew::Get()
{
	return *g_App;
}

std::shared_ptr<WindowClass> ApplicationNew::CreateRenderWindow(std::wstring& windowName, UINT clientWidth, UINT clientHeight, bool vsync)
{
	WindowNameMap::iterator windowIter = g_WindowByName.find(windowName);
	if (windowIter != g_WindowByName.end())
	{
		return windowIter->second;
	}

	RECT windowRect = { 0, 0, static_cast<LONG>(clientWidth), static_cast<LONG>(clientHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindowEx(0, WINDOW_CLASS_NAME, windowName.c_str(),
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, m_hInstance, nullptr);

	if (hwnd == NULL)
	{
		MessageBoxA(nullptr, "Could not create the window", "Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	WindowPtr pWindow = std::make_shared<MakeWindow>(hwnd, windowName, clientWidth, clientHeight, vsync);
	g_Windows.insert(WindowMap::value_type(hwnd, pWindow));
	g_WindowByName.insert(WindowNameMap::value_type(windowName, pWindow));

	return pWindow;
}

void ApplicationNew::DestroyWindow(const std::wstring& windowName)
{
	WindowPtr window = GetWindowByName(windowName);
	if (window)
	{
		DestroyWindow(window);
	}
}

void ApplicationNew::DestroyWindow(std::shared_ptr<WindowClass> window)
{
	if (window)
	{
		window->Cleanup();
	}
}

std::shared_ptr<WindowClass> ApplicationNew::GetWindowByName(const std::wstring& windowName)
{
	std::shared_ptr<WindowClass> window;
	WindowNameMap::iterator iter = g_WindowByName.find(windowName);
	if (iter != g_WindowByName.end())
	{
		window = iter->second;
	}

	return window;
}

int ApplicationNew::Run(std::shared_ptr<EngineBase> pEngineBase)
{
	if (!pEngineBase->Initialise()) return 1;
	if (!pEngineBase->LoadContent()) return 2;

	MSG msg = { 0 };

	WindowPtr pWindow;
	{
		pWindow = GetWindowByName(L"DX11 Engine");
	}

	BOOL done = false;
	Timer timer = Timer();
	timer.Reset();

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
			break;
		}

		timer.Tick();

		InputHandler::UpdateInputState();

		UpdateEvent updateEvent(timer.GetDeltaSeconds(), timer.GetTotalSeconds());
		pWindow->OnUpdate(updateEvent);

		RenderEvent renderEvent(timer.GetDeltaSeconds(), timer.GetTotalSeconds());
		pWindow->OnRender(renderEvent);
	}

	pEngineBase->UnloadContent();
	pEngineBase->Cleanup();

	return static_cast<int>(msg.wParam);
}

void ApplicationNew::Quit(int exitCode)
{
	PostQuitMessage(exitCode);
}

ApplicationNew::ApplicationNew(HINSTANCE hinst)
	: m_hInstance(hinst)
	, m_ConfigLoader(nullptr)
	, m_Debug(nullptr)
{
}

ApplicationNew::~ApplicationNew()
{
	m_Context.Reset();
	m_Device.Reset();
}

void ApplicationNew::Initialise()
{
	m_ConfigLoader = new C_ConfigLoader(std::string("settings.xml"));
	m_ConfigLoader->Initialise();

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	
	WNDCLASSEX windowClass = { 0 };

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.hInstance = m_hInstance;
	windowClass.hIcon = (HICON)LoadImage(m_hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	windowClass.hCursor = ::LoadCursorW(m_hInstance, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = WINDOW_CLASS_NAME;
	windowClass.hIconSm = (HICON)LoadImage(m_hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);

	if (!RegisterClassEx(&windowClass))
	{
		MessageBoxA(nullptr, "Unable to register the window class", "Error", MB_OK | MB_ICONERROR);
	}

	CreateDevice(nullptr);
	
#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	HRESULT hr = m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_Debug);
#endif
}

void ApplicationNew::Cleanup()
{
	m_Context->ClearState();
	m_Context->Flush();

	delete m_ConfigLoader;
	m_ConfigLoader = nullptr;

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	m_Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
}

ComPtr<IDXGIAdapter1> ApplicationNew::GetAdapter()
{
	ComPtr<IDXGIFactory1> dxgiFactory;
	UINT createFactoryFlags = 0;

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	createFactoryFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory));

	ComPtr<IDXGIAdapter1> dxgiAdapter1;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	SIZE_T maxDedicatedVideoMemory = 0;
	for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
		{
			D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
			
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D11CreateDevice(nullptr, driverType, nullptr, createFactoryFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &m_Device, NULL, &m_Context)) 
				&& dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;

				m_Device->Release();
				m_Context->Release();
				//ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
			}
		}
	}

	dxgiFactory->Release();

	return dxgiAdapter1;
}

void ApplicationNew::CreateDevice(ComPtr<IDXGIAdapter1> adapter)
{
	UINT flags = 0;

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &m_Device, NULL, &m_Context));

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	char const deviceName[] = "D3D11 Device";
	char const contextName[] = "D3D11 Context";
	m_Device->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(deviceName) - 1, deviceName);
	m_Context->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(contextName) - 1, contextName);
#endif

}

static void RemoveWindow(HWND hwnd)
{
	WindowMap::iterator iter = g_Windows.find(hwnd);
	if (iter != g_Windows.end())
	{
		WindowPtr window = iter->second;
		g_WindowByName.erase(window->GetWindowName());
		g_Windows.erase(iter);
	}
}

MouseButtonEvent::MouseButton DecodeMouseButton(UINT messageID)
{
	MouseButtonEvent::MouseButton mouseButton = MouseButtonEvent::MouseButton::None;
	switch (messageID)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	{
		mouseButton = MouseButtonEvent::MouseButton::Left;
	}
	break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	{
		mouseButton = MouseButtonEvent::MouseButton::Right;
	}
	break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	{
		mouseButton = MouseButtonEvent::MouseButton::Middle;
	}
	break;
	}

	return mouseButton;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowPtr pWindow;
	{
		WindowMap::iterator iter = g_Windows.find(hwnd);
		if (iter != g_Windows.end())
		{
			pWindow = iter->second;
		}
	}

	if (pWindow)
	{
		PAINTSTRUCT ps;
		HDC hdc;
		switch (message)
		{
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			return 0;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			MSG charMsg;
			unsigned int c = 0;

			if (PeekMessage(&charMsg, hwnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
			{
				GetMessage(&charMsg, hwnd, 0, 0);
				c = static_cast<unsigned int>(charMsg.wParam);
			}

			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

			KeyCode::Key key = (KeyCode::Key)wParam;
			unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
			KeyEvent keyEvent(key, c, KeyEvent::KeyState::Pressed, ctrl, shift, alt);
			pWindow->OnKeyPressed(keyEvent);
		}
		break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
			bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
			KeyCode::Key key = (KeyCode::Key)wParam;
			unsigned int c = 0;
			unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

			unsigned char keyboardState[256];
			ThrowIfFailedBool(GetKeyboardState(keyboardState));
			wchar_t translatedCharacters[4];

			if (int result = ToUnicodeEx(static_cast<UINT>(wParam), scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
			{
				c = translatedCharacters[0];
			}

			KeyEvent keyEvent(key, c, KeyEvent::KeyState::Released, ctrl, shift, alt);
			pWindow->OnKeyReleased(keyEvent);
		}
		break;

		case WM_SYSCHAR:
			break;

		case WM_MOUSEMOVE:
		{
			bool lmButton = (wParam & MK_LBUTTON) != 0;
			bool rmButton = (wParam & MK_RBUTTON) != 0;
			bool mmButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool ctrl = (wParam & MK_CONTROL) != 0;

			POINT currMousePos;
			ThrowIfFailedBool(GetCursorPos(&currMousePos));

			if (hwnd != nullptr)
			{
				ThrowIfFailedBool(ScreenToClient(hwnd, &currMousePos));
			}
			
			MouseMotionEvent motionEvent(lmButton, mmButton, rmButton, ctrl, shift, (int)currMousePos.x, (int)currMousePos.y);
			pWindow->OnMouseMoved(motionEvent);
		}
		break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			bool lmButton = (wParam & MK_LBUTTON) != 0;
			bool rmButton = (wParam & MK_RBUTTON) != 0;
			bool mmButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool ctrl = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			MouseButtonEvent mouseButtonEvent(DecodeMouseButton(message), MouseButtonEvent::ButtonState::Pressed, lmButton, mmButton, rmButton, ctrl, shift, x, y);
			pWindow->OnMouseButtonDown(mouseButtonEvent);
		}
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			bool lButton = (wParam & MK_LBUTTON) != 0;
			bool rButton = (wParam & MK_RBUTTON) != 0;
			bool mButton = (wParam & MK_MBUTTON) != 0;
			bool shift = (wParam & MK_SHIFT) != 0;
			bool control = (wParam & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			MouseButtonEvent mouseButtonEvent(DecodeMouseButton(message), MouseButtonEvent::ButtonState::Released, lButton, mButton, rButton, control, shift, x, y);
			pWindow->OnMouseButtonUp(mouseButtonEvent);
		}
		break;
		case WM_MOUSEWHEEL:
		{
			// The distance the mouse wheel is rotated.
			// A positive value indicates the wheel was rotated to the right.
			// A negative value indicates the wheel was rotated to the left.
			float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
			short keyStates = (short)LOWORD(wParam);

			bool lButton = (keyStates & MK_LBUTTON) != 0;
			bool rButton = (keyStates & MK_RBUTTON) != 0;
			bool mButton = (keyStates & MK_MBUTTON) != 0;
			bool shift = (keyStates & MK_SHIFT) != 0;
			bool control = (keyStates & MK_CONTROL) != 0;

			int x = ((int)(short)LOWORD(lParam));
			int y = ((int)(short)HIWORD(lParam));

			// Convert the screen coordinates to client coordinates.
			POINT clientToScreenPoint{};
			clientToScreenPoint.x = x;
			clientToScreenPoint.y = y;
			ScreenToClient(hwnd, &clientToScreenPoint);

			MouseWheelEvent mouseWheelEvent(zDelta, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y);
			pWindow->OnMouseWheel(mouseWheelEvent);
		}
		break;
		case WM_SIZE:
		{
			UINT width = static_cast<UINT>(LOWORD(lParam));
			UINT height = static_cast<UINT>(HIWORD(lParam));
			pWindow->OnResize(width, height);
		}
		break;

		case WM_DESTROY:
		{
			RemoveWindow(hwnd);
			if (g_Windows.empty())
			{
				PostQuitMessage(0);
			}
		}
		break;

		default:
			return DefWindowProcW(hwnd, message, wParam, lParam);
		}
	}
	else
	{
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}

	return DefWindowProcW(hwnd, message, wParam, lParam);
}
