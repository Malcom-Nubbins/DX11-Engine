#include "EngineBase.h"
#include "Globals/stdafx.h"
#include "ApplicationNew.h"
#include "Handlers/System Handlers/WindowClass.h"
#include "Handlers/System Handlers/InputHandler.h"

EngineBase::EngineBase(const std::wstring& name, UINT width, UINT height, bool vsync)
	: m_Name(name)
	, m_Width(width)
	, m_Height(height)
	, m_vSync(vsync)
{
}

EngineBase::~EngineBase()
{
}

bool EngineBase::Initialise()
{
	using namespace DirectX;
	if (!DirectX::XMVerifyCPUSupport())
	{
		MessageBoxA(nullptr, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	m_AppWindow = ApplicationNew::Get().CreateRenderWindow(m_Name, m_Width, m_Height, m_vSync);
	m_AppWindow->RegisterCallbacks(shared_from_this());
	m_AppWindow->Show();

	return true;
}

void EngineBase::Cleanup()
{
	ApplicationNew::Get().DestroyWindow(m_AppWindow);
	m_AppWindow.reset();
}

void EngineBase::OnUpdate(UpdateEvent& e)
{
}

void EngineBase::OnRender(RenderEvent& e)
{
}

void EngineBase::OnKeyPressed(KeyEvent& e)
{
	if (e.Key == KeyCode::Key::Escape)
	{
		ApplicationNew::Get().Quit();
	}

	if (e.Key == KeyCode::Key::F1)
	{
		ApplicationNew::Get().ReloadConfigs();
		OnConfigsReloaded();
	}
}

void EngineBase::OnKeyReleased(KeyEvent& e)
{
}

void EngineBase::OnMouseMoved(MouseMotionEvent& e)
{
}

void EngineBase::OnMouseButtonDown(MouseButtonEvent& e)
{
}

void EngineBase::OnMouseButtonUp(MouseButtonEvent& e)
{
}

void EngineBase::OnMouseWheel(MouseWheelEvent& e)
{
}

void EngineBase::PreOnResize()
{
}

void EngineBase::OnResize(UINT width, UINT height)
{
	m_Width = width;
	m_Height = height;
}

void EngineBase::OnWindowDestroyed()
{
	UnloadContent();
}

void EngineBase::OnConfigsReloaded()
{
}
