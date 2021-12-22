#include "SceneHandler.h"

#include "../ApplicationNew.h"
#include "../Core/Loaders/ConfigLoader.h"
#include "../Core/Loaders/XMLLoader/rapidxml.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_ext.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_print.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_utils.hpp"
#include <vector>
#include <codecvt>

SceneHandler::SceneHandler()
{
}

SceneHandler::~SceneHandler()
{
}

void SceneHandler::Cleanup()
{
	for (auto& scene : m_Scenes)
	{
		scene->Cleanup();
		scene = nullptr;
	}

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
	using namespace rapidxml;
	using namespace std;
	auto configLoader = ApplicationNew::Get().GetConfigLoader();

	S_ConfigInfo const scenesConfig = configLoader->GetConfigByName("ScenesConfig");
	if (!scenesConfig.m_ConfigFilename.empty())
	{
		wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		string configFileName(converter.to_bytes(scenesConfig.m_ConfigFilename));
		string filePath(configLoader->GetSettingStringValue(SettingType::Engine, "ConfigDir"));

		string const fullPath(filePath + configFileName);

		if (!configLoader->CheckConfigExists(fullPath))
		{
			CreateDefaultSceneConfig(fullPath);
		}

		file<> xmlFile(fullPath.c_str());
		xml_document<> doc;

		doc.parse<0>(xmlFile.data());

		xml_node<>* rootNode = doc.first_node("Scenes");

		for (xml_node<>* sceneNode = rootNode->first_node("Scene"); sceneNode; sceneNode = sceneNode->next_sibling())
		{
			std::string const sceneName(sceneNode->first_attribute("name")->value());

			Scene* newScene = new Scene(sceneName.c_str());
			newScene->InitialiseScene(doc, *sceneNode);

			m_Scenes.emplace_back(move(newScene));
		}
	}

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

void SceneHandler::CreateDefaultSceneConfig(std::string const& inConfigFilename)
{
}
