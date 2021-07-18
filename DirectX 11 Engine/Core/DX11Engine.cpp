#include "DX11Engine.h"
#include "ApplicationNew.h"

static DX11Engine* g_Engine = nullptr;

DX11Engine::DX11Engine(const std::wstring& name, UINT width, UINT height, bool vsync)
	: super(name, width, height, vsync)
	, m_ContentLoaded(false)
	, m_BufferClass(nullptr)
	, m_RenderClass(nullptr)
	, m_MainScene(nullptr)
	, m_Player(nullptr)
	, m_ShaderClass(nullptr)
	, m_TestingScene(nullptr)
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

	m_Player = new Player();
	m_Player->Initialise();

	/*m_MainScene = new MainScene(*m_Player);
	m_MainScene->InitialiseScene(m_AppWindow->GetWindowWidth(), m_AppWindow->GetWindowHeight());*/

	m_TestingScene = new TestingScene(*m_Player);
	m_TestingScene->InitialiseScene(m_AppWindow->GetWindowWidth(), m_AppWindow->GetWindowHeight());

	m_UI = new UserInterface(m_Player->GetCamera());
	m_UI->Initialise();

	m_UI->AddBitmapToUI(XMFLOAT2(200, 200), XMFLOAT2(5, 5), m_TextureHandler->GetTextureByName("Snow"));
	m_UI->AddBitmapToUI(XMFLOAT2(200, 200), XMFLOAT2(5, 210), m_TextureHandler->GetTextureByName("StoneColour"));

	m_ContentLoaded = true;

	return m_ContentLoaded;
}

void DX11Engine::UnloadContent()
{
}

void DX11Engine::Cleanup()
{
	if (m_MainScene != nullptr)
	{
		m_MainScene->Cleanup();
		m_MainScene = nullptr;
	}
	
	if (m_TestingScene != nullptr)
	{
		m_TestingScene->Cleanup();
		m_TestingScene = nullptr;
	}

	m_UI->Cleanup();
	m_UI = nullptr;

	m_Player->Cleanup();
	m_Player = nullptr;

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
}

void DX11Engine::OnUpdate(UpdateEvent& e)
{
	super::OnUpdate(e);
	if (m_ContentLoaded)
	{
		m_Player->Update(e.ElapsedTime);
		m_UI->Update(e.ElapsedTime);

		if (m_TestingScene != nullptr)
		{
			m_TestingScene->Update(e);
		}

		if (m_MainScene != nullptr)
		{
			m_MainScene->Update(e);
		}
	}
}

void DX11Engine::OnRender(RenderEvent& e)
{
	if (m_ContentLoaded)
	{
		if (m_TestingScene != nullptr)
		{
			m_TestingScene->Draw();
		}

		if (m_MainScene != nullptr)
		{
			m_MainScene->Draw();
		}

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
	if (m_TestingScene != nullptr)
	{
		m_TestingScene->PreResizeViews();
	}

	if (m_MainScene != nullptr)
	{
		m_MainScene->PreResizeViews();
	}
}

void DX11Engine::OnResize(UINT width, UINT height)
{
	if (m_TestingScene != nullptr)
	{
		m_TestingScene->ResizeViews(static_cast<float>(width), static_cast<float>(height));
	}

	if (m_MainScene != nullptr)
	{
		m_MainScene->ResizeViews(static_cast<float>(width), static_cast<float>(height));
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
