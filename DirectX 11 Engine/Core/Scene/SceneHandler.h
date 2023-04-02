#pragma once
#include "../Globals/stdafx.h"
#include "Scene Elements/SceneElement.h"
#include "../Core/Scene/Scene.h"
#include "../Core/Loaders/IConfigInterface.h"
#include <vector>
#include <thread>

class SceneHandler : public IConfigInterface
{
public:
	SceneHandler();
	~SceneHandler();

	void Cleanup();

	void PreResize();
	void ResizeViews(float const newWidth, float const newHeight);

	void LoadScenesFromSceneInfo(const SceneInfo& SceneToLoad);

	Scene* GetCurrentScene() const { return m_CurrentScene; }

	void GoToNextScene();
	void GoToPreviousScene();

	void Upate(UpdateEvent& e);
	void Draw();

protected:
	void CreateConfig() override;
	void LoadConfig() override;

private:
	
	std::vector<SceneInfo> m_SceneInfos;
	Scene* m_CurrentScene;

	int m_CurrSceneIndex;
};

