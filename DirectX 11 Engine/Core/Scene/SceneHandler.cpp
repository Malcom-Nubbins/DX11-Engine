#include "SceneHandler.h"

SceneHandler::SceneHandler()
{
}

SceneHandler::~SceneHandler()
{
}

void SceneHandler::Cleanup()
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->Cleanup();
		m_CurrentScene = nullptr;
	}

	m_Scenes.clear();
}

void SceneHandler::PreResize()
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->PreResizeViews();
	}
}

void SceneHandler::ResizeViews(float const newWidth, float const newHeight)
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->ResizeViews(newWidth, newHeight);
	}
}

void SceneHandler::LoadScenesFromConfig(Player& player, float const width, float const height)
{
	Scene* newScene = new Scene(player);
	newScene->InitialiseScene(width, height);

	m_Scenes.emplace_back(std::move(newScene));

	m_CurrentScene = m_Scenes.front();
}

void SceneHandler::Upate(UpdateEvent& e)
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->Update(e);
	}
}

void SceneHandler::Draw()
{
}
