#include "WindowClass.h"
#include "../../ApplicationNew.h"
#include "../../EngineBase.h"
#include "../../Globals/AppValues.h"

WindowClass::WindowClass(HWND& hwnd, std::wstring& windowName, UINT clientWidth, UINT clientHeight, bool vsync)
	: m_hWnd(hwnd)
	, m_WindowCaption(windowName)
	, m_WindowWidth(clientWidth)
	, m_WindowHeight(clientHeight)
	, m_ScreenCentre()
	, m_WindowRect()
	, m_FrameCounter(0)
	, m_VSync(vsync)
	, m_Fullscreen(false)
	, m_BackBuffer(nullptr)
	, m_BackBufferTex2D(nullptr)
{
	ApplicationNew& app = ApplicationNew::Get();

	CreateSwapChain();

	UpdateRenderTargetViews();

	m_ScreenCentre.x = m_WindowWidth / 2;
	m_ScreenCentre.y = m_WindowHeight / 2;
}

WindowClass::~WindowClass()
{
	m_BackBuffer.Reset();
	m_BackBufferTex2D.Reset();
	m_SwapChain.Reset();
}

void WindowClass::RegisterCallbacks(std::shared_ptr<EngineBase> pEngineBase)
{
	m_EngineBase = pEngineBase;
}

void WindowClass::OnUpdate(UpdateEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		m_FrameCounter++;

		UpdateEvent updateEvent(e.ElapsedTime, e.TotalTime);
		pEngine->OnUpdate(updateEvent);
	}
}

void WindowClass::OnRender(RenderEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		RenderEvent renderEvent(e.ElapsedTime, e.TotalTime);
		pEngine->OnRender(renderEvent);
	}
}

void WindowClass::OnKeyPressed(KeyEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		pEngine->OnKeyPressed(e);
	}
}

void WindowClass::OnKeyReleased(KeyEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		pEngine->OnKeyReleased(e);
	}
}

void WindowClass::OnMouseMoved(MouseMotionEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		pEngine->OnMouseMoved(e);
	}
}

void WindowClass::OnMouseButtonDown(MouseButtonEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		pEngine->OnMouseButtonDown(e);
	}
}

void WindowClass::OnMouseButtonUp(MouseButtonEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		pEngine->OnMouseButtonUp(e);
	}
}

void WindowClass::OnMouseWheel(MouseWheelEvent& e)
{
	if (auto pEngine = m_EngineBase.lock())
	{
		pEngine->OnMouseWheel(e);
	}
}

void WindowClass::OnResize(UINT width, UINT height)
{
	if (m_WindowWidth != width || m_WindowHeight != height)
	{
		if (auto pEngine = m_EngineBase.lock())
		{
			ID3D11RenderTargetView* nullViews[] = { nullptr };

			ApplicationNew::Get().GetContext()->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);

			pEngine->PreOnResize();

			ApplicationNew::Get().GetContext()->Flush();

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
			ComPtr<ID3D11Debug> const& debugPtr = ApplicationNew::Get().GetDebug();
			if (debugPtr)
			{
				debugPtr->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			}
#endif
		}

		m_WindowWidth = std::max(1, static_cast<int>(width));
		m_WindowHeight = std::max(1, static_cast<int>(height));

		m_ScreenCentre.x = m_WindowWidth / 2;
		m_ScreenCentre.y = m_WindowHeight / 2;

		m_BackBufferTex2D.Reset();
		m_BackBuffer.Reset();

		ApplicationNew::Get().GetContext()->ClearState();
		ApplicationNew::Get().GetContext()->Flush();

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
		ComPtr<ID3D11Debug> const& debugPtr = ApplicationNew::Get().GetDebug();
		if (debugPtr)
		{
			debugPtr->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
#endif

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ThrowIfFailed(m_SwapChain->GetDesc(&swapChainDesc));
		ThrowIfFailed(m_SwapChain->ResizeBuffers(0, m_WindowWidth, m_WindowHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

		UpdateRenderTargetViews();
	}

	if (auto pEngine = m_EngineBase.lock())
	{
		pEngine->OnResize(width, height);
	}
}

void WindowClass::CreateSwapChain()
{
	ApplicationNew& app = ApplicationNew::Get();
	ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;

#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

	DXGI_SWAP_CHAIN_DESC1 sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.Width = m_WindowWidth;
	sd.Height = m_WindowHeight;
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.Stereo = false;
	sd.SampleDesc = { 1, 0 };
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags = 0;

	ID3D11Device* device = app.GetDevice().Get();

	ComPtr<IDXGISwapChain1> swapChain1;
	ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(device, m_hWnd, &sd, nullptr, nullptr, &swapChain1));

	ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
	ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

	m_SwapChain = dxgiSwapChain4;

	//dxgiSwapChain4->Release();
}

void WindowClass::UpdateRenderTargetViews()
{
	auto device = ApplicationNew::Get().GetDevice();

	ID3D11Texture2D* backBuffer;
	ThrowIfFailed(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer));
	
	m_BackBufferTex2D = backBuffer;

	ID3D11RenderTargetView* pBackBuffer = m_BackBuffer.Get();
	device->CreateRenderTargetView(backBuffer, nullptr, &pBackBuffer);

	m_BackBuffer = pBackBuffer;

	pBackBuffer->Release();
	pBackBuffer = nullptr;

	backBuffer->Release();
	backBuffer = nullptr;

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	char const backBufferName[] = "BackBuffer";
	m_BackBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(backBufferName) - 1, backBufferName);

	char const backBufferTexName[] = "BackBufferTex";
	m_BackBufferTex2D->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(backBufferTexName) - 1, backBufferTexName);
#endif
}

void WindowClass::Cleanup()
{
	if (auto engine = m_EngineBase.lock())
	{
		engine->OnWindowDestroyed();
	}

	/*m_BackBuffer->Release();
	m_BackBufferTex2D->Release();*/

	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}

	//m_SwapChain->Release();
	//m_SwapChain = nullptr;

	ApplicationNew::Get().GetContext()->ClearState();
	ApplicationNew::Get().GetContext()->Flush();

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	ComPtr<ID3D11Debug> const& debugPtr = ApplicationNew::Get().GetDebug();
	if (debugPtr)
	{
		debugPtr->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif
}

void WindowClass::Show()
{
	::ShowWindow(m_hWnd, SW_SHOW);
}

void WindowClass::Hide()
{
	::ShowWindow(m_hWnd, SW_HIDE);
}

void WindowClass::SetFullscreen(bool fullscreen)
{
	if (m_Fullscreen != fullscreen)
	{
		m_Fullscreen = fullscreen;

		if (m_Fullscreen)
		{
			::GetWindowRect(m_hWnd, &m_WindowRect);

			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

			::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);

			HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			::SetWindowPos(m_hWnd, HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWnd, SW_MAXIMIZE);
		}
		else
		{
			::SetWindowLongW(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(m_hWnd, HWND_TOP,
				m_WindowRect.left,
				m_WindowRect.top,
				m_WindowRect.right - m_WindowRect.left,
				m_WindowRect.bottom - m_WindowRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			::ShowWindow(m_hWnd, SW_NORMAL);
		}
	}
}

UINT WindowClass::Present()
{
	UINT syncInterval = m_VSync ? 1 : 0;
	ThrowIfFailed(m_SwapChain->Present(syncInterval, 0));
	return 0;
}
