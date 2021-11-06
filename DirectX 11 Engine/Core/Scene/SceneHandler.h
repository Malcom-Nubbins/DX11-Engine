#pragma once
#include "../Globals/stdafx.h"
#include "Scene Elements/SceneElement.h"
#include <vector>

struct S_Scene final
{
	std::vector<SceneElement> m_Elements;
};

class SceneHandler
{
public:
	SceneHandler();
	~SceneHandler();

	void LoadScenesFromConfig();

	//S_Scene GetCurrentScene() const { return m_CurrentScene; }

private:
	std::vector<S_Scene> m_Scenes;
	S_Scene m_CurrentScene;
};

