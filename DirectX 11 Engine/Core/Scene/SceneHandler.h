#pragma once
#include "../Globals/stdafx.h"
#include "Scene Elements/SceneElement.h"
#include "../Core/Scene/Scene.h"
#include <vector>
#include <thread>

class SceneHandler
{
public:
	SceneHandler();
	~SceneHandler();

	void Cleanup();

	void PreResize();
	void ResizeViews(float const newWidth, float const newHeight);

	void FindAllScenes();

	void LoadScenesFromName(const std::string SceneToLoad);

	Scene* GetCurrentScene() const { return m_CurrentScene; }

	void GoToNextScene();
	void GoToPreviousScene();

	void Upate(UpdateEvent& e);
	void Draw();

private:
	void CreateDefaultSceneConfig(std::string const& inConfigFilename);

	std::vector<std::string> m_Scenes;
	Scene* m_CurrentScene;

	int m_CurrSceneIndex;
};

