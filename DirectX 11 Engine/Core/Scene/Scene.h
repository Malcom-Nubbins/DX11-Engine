﻿#pragma once
#include "../Globals/Structs.h"
#include "../Timer.h"
#include "Player.h"
#include "../Handlers/SystemHandlers.h"
#include "../Loaders/Terrain Generation/FlatTerrain.h"
#include "../Core/Loaders/XMLLoader/rapidxml.hpp"

class Scene
{
protected:
	std::string m_SceneName;
	std::vector<SceneElement*> m_SceneElements;

public:
	Scene(char const* sceneName);
	virtual ~Scene();
	void Cleanup();

	void PreResizeViews();
	void ResizeViews(float windowWidth, float windowHeight);

	void InitialiseScene(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& sceneNode);

	void Update(UpdateEvent& e);
	void Draw();

public:
	std::vector<SceneElement*> GetAllSceneElements() const { return m_SceneElements; }
	//Player* GetPlayer() const { return m_Player; }
};
