#include "DX11Engine.h"
#include "ApplicationNew.h"

static DX11Engine* g_Engine = nullptr;

DX11Engine::DX11Engine(const std::wstring& name, UINT width, UINT height, bool vsync)
	: super(name, width, height, vsync)
	, m_ContentLoaded(false)
	, m_BufferClass(nullptr)
	, m_RenderClass(nullptr)
	, m_SceneHandler(nullptr)
	, m_Player(nullptr)
	, m_ShaderClass(nullptr)
	, m_GraphicsHandler(nullptr)
	, m_TextureHandler(nullptr)
	, m_UI(nullptr)
	, m_ToggleCooldown(0.0f)
{
	g_Engine = this;
}

bool DX11Engine::LoadContent()
{
	HRESULT hr;
	auto device = ApplicationNew::Get().GetDevice();
	auto context = ApplicationNew::Get().GetContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_RenderClass = new RenderClass();
	m_ShaderClass = new ShaderClass();

	hr = m_ShaderClass->CreateSamplerStates();

	if (FAILED(hr))
	{
		return false;
	}

	m_BufferClass = new BufferClass();

	m_TextureHandler = new TextureHandler();
	m_TextureHandler->LoadAllTextures();

	m_GraphicsHandler = new GraphicsHandler();
	m_GraphicsHandler->Init(m_AppWindow->GetWindowWidth(), m_AppWindow->GetWindowHeight());

	m_Player = new Player();
	m_Player->Initialise();

	m_SceneHandler = new SceneHandler();

	/*m_MainScene = new MainScene(*m_Player);
	m_MainScene->InitialiseScene(m_AppWindow->GetWindowWidth(), m_AppWindow->GetWindowHeight());*/

	/*m_TestingScene = new TestingScene(*m_Player);
	m_TestingScene->InitialiseScene(m_AppWindow->GetWindowWidth(), m_AppWindow->GetWindowHeight());*/

	m_UI = new UserInterface(m_Player->GetCamera());
	m_UI->Initialise();

	m_ContentLoaded = true;

	ShowCursor(false);

	return m_ContentLoaded;
}

void DX11Engine::UnloadContent()
{
}

void DX11Engine::Cleanup()
{
	if (m_SceneHandler != nullptr)
	{
		m_SceneHandler->Cleanup();
		m_SceneHandler = nullptr;
	}

	m_UI->Cleanup();
	m_UI = nullptr;

	m_Player->Cleanup();
	m_Player = nullptr;

	m_GraphicsHandler->Cleanup();
	m_GraphicsHandler = nullptr;

	m_TextureHandler->Cleanup();
	m_TextureHandler = nullptr;

	m_RenderClass->Cleanup();
	m_RenderClass = nullptr;

	m_ShaderClass->Cleanup();
	m_ShaderClass = nullptr;

	ModelLoader::UnloadAllModels();

	super::Cleanup();
}

DX11Engine& DX11Engine::Get()
{
	if (g_Engine)
	{
		return *g_Engine;
	}

	throw new std::exception("FATAL: DX11 Engine was not initialised!!");
}

void DX11Engine::OnUpdate(UpdateEvent& e)
{
	super::OnUpdate(e);
	if (m_ContentLoaded)
	{
		m_Player->Update(e.ElapsedTime);
		m_UI->Update(e.ElapsedTime);

		if (m_SceneHandler != nullptr)
		{
			m_SceneHandler->Upate(e);
		}

		m_GraphicsHandler->Update(e);
	}
}

void DX11Engine::OnRender(RenderEvent& e)
{
	if (m_ContentLoaded)
	{
		m_GraphicsHandler->Draw();

		m_UI->Draw();

		m_AppWindow->Present();
	}
}

void DX11Engine::OnKeyPressed(KeyEvent& e)
{
	super::OnKeyPressed(e);
	m_Player->OnPlayerMovementKeyPressed(e);
}

void DX11Engine::OnKeyReleased(KeyEvent& e)
{
	super::OnKeyReleased(e);
	m_Player->OnPlayerMovementKeyReleased(e);
}

void DX11Engine::OnMouseWheel(MouseWheelEvent& e)
{
}

void DX11Engine::PreOnResize()
{
	if (m_SceneHandler != nullptr)
	{
		m_SceneHandler->PreResize();
	}

	if (m_GraphicsHandler != nullptr)
	{
		m_GraphicsHandler->PreResizeViews();
	}
}

void DX11Engine::OnResize(UINT width, UINT height)
{
	super::OnResize(width, height);

	if (m_Player != nullptr)
	{
		m_Player->ResetPlayerCamera(static_cast<float>(width), static_cast<float>(height));
	}

	if (m_SceneHandler != nullptr)
	{
		m_SceneHandler->ResizeViews(static_cast<float>(width), static_cast<float>(height));
	}

	if (m_GraphicsHandler != nullptr)
	{
		m_GraphicsHandler->ResizeViews(static_cast<float>(width), static_cast<float>(height));
	}

	if (m_UI != nullptr)
	{
		m_UI->Resize(static_cast<float>(width), static_cast<float>(height));
	}
}

void DX11Engine::OnMouseMoved(MouseMotionEvent& e)
{
	super::OnMouseMoved(e);
	m_Player->UpdatePlayerLookDirection(e);
}

void DX11Engine::OnMouseButtonDown(MouseButtonEvent& e)
{
	super::OnMouseButtonDown(e);
	m_Player->OnMouseButtonDown(e);
}

void DX11Engine::OnMouseButtonUp(MouseButtonEvent& e)
{
	super::OnMouseButtonUp(e);
	m_Player->OnMouseButtonUp(e);
}

void DX11Engine::OnConfigsReloaded()
{
	super::OnConfigsReloaded();
	if (m_UI != nullptr)
	{
		m_UI->ReloadUI();
	}
}
