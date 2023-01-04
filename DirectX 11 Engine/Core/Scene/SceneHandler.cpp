#include "SceneHandler.h"

#include "../ApplicationNew.h"
#include "../Core/Loaders/ConfigLoader.h"
#include "../Core/Loaders/XMLLoader/rapidxml.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_ext.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_print.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_utils.hpp"
#include <codecvt>

SceneHandler::SceneHandler() : m_CurrentScene(nullptr), m_CurrSceneIndex(0)
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

void SceneHandler::FindAllScenes()
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

			//Scene* newScene = new Scene(sceneName.c_str());
			//newScene->InitialiseScene(doc, *sceneNode);

			m_Scenes.emplace_back(move(sceneName));
		}
	}

	if (m_Scenes.size() > 0)
	{
		LoadScenesFromName(m_Scenes.front());
	}
}

void SceneHandler::LoadScenesFromName(const std::string SceneToLoad)
{	
	if (m_CurrentScene != nullptr)
	{
		// Unload and nullify
		m_CurrentScene->Unload();
		m_CurrentScene->Cleanup();

		m_CurrentScene = nullptr;
	}

	m_CurrentScene = new Scene(SceneToLoad.c_str());
	m_CurrentScene->InitialiseScene();
}

void SceneHandler::GoToNextScene()
{
	++m_CurrSceneIndex;

	if (m_CurrSceneIndex > m_Scenes.size() - 1)
	{
		m_CurrSceneIndex = 0;
	}

	LoadScenesFromName(m_Scenes[m_CurrSceneIndex]);
}

void SceneHandler::GoToPreviousScene()
{
	--m_CurrSceneIndex;

	if (m_CurrSceneIndex < 0)
	{
		m_CurrSceneIndex = m_Scenes.size() - 1;
	}

	LoadScenesFromName(m_Scenes[m_CurrSceneIndex]);
}

void SceneHandler::Upate(UpdateEvent& e)
{
	if (InputHandler::IsKeyUp(Keys::OemCloseBrackets))
	{
		GoToNextScene();
	}
	else if (InputHandler::IsKeyUp(Keys::OemOpenBrackets))
	{
		GoToPreviousScene();
	}

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
	using namespace rapidxml;
	using namespace std;

	ofstream newDefaultScenesConfig(inConfigFilename);

	xml_document<> doc;
	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
	doc.append_node(decl);

	xml_node<>* root = doc.allocate_node(node_element, "Scenes");
	doc.append_node(root);

	// Default scene
	{
		xml_node<>* defaultScene = doc.allocate_node(node_element, "Scene");
		defaultScene->append_attribute(doc.allocate_attribute("name", "Default"));
		root->append_node(defaultScene);

		// Default scene elements
		{
			xml_node<>* defaultElements = doc.allocate_node(node_element, "Elements");
			defaultScene->append_node(defaultElements);

			xml_node<>* defaultSphereObject = doc.allocate_node(node_element, "Object");
			defaultSphereObject->append_attribute(doc.allocate_attribute("name", "Sphere"));
			defaultSphereObject->append_attribute(doc.allocate_attribute("Amount", "1"));
			defaultSphereObject->append_attribute(doc.allocate_attribute("IsFlatPlane", "0"));
			defaultSphereObject->append_attribute(doc.allocate_attribute("IsTerrain", "0"));
			defaultSphereObject->append_attribute(doc.allocate_attribute("GenWidth", "0"));
			defaultSphereObject->append_attribute(doc.allocate_attribute("GenHeight", "0"));
			defaultSphereObject->append_attribute(doc.allocate_attribute("GenSize", "0"));
			defaultElements->append_node(defaultSphereObject);

			// Sphere details
			{
				xml_node<>* transform = doc.allocate_node(node_element, "Transform");
				defaultSphereObject->append_node(transform);
				
				// Transform
				{
					xml_node<>* Position = doc.allocate_node(node_element, "Position");
					Position->append_attribute(doc.allocate_attribute("x", "0.0"));
					Position->append_attribute(doc.allocate_attribute("y", "1.0"));
					Position->append_attribute(doc.allocate_attribute("z", "0.0"));
					transform->append_node(Position);

					xml_node<>* Scale = doc.allocate_node(node_element, "Scale");
					Scale->append_attribute(doc.allocate_attribute("x", "1.0"));
					Scale->append_attribute(doc.allocate_attribute("y", "1.0"));
					Scale->append_attribute(doc.allocate_attribute("z", "1.0"));
					transform->append_node(Scale);

					xml_node<>* Rotation = doc.allocate_node(node_element, "Rotation");
					Rotation->append_attribute(doc.allocate_attribute("x", "0.0"));
					Rotation->append_attribute(doc.allocate_attribute("y", "0.0"));
					Rotation->append_attribute(doc.allocate_attribute("z", "0.0"));
					transform->append_node(Rotation);
				}

				xml_node<>* appearance = doc.allocate_node(node_element, "Appearance");
				appearance->append_attribute(doc.allocate_attribute("material", "Default"));
				appearance->append_attribute(doc.allocate_attribute("model", "spherex.obj"));
				appearance->append_attribute(doc.allocate_attribute("texture", "Default"));
				appearance->append_attribute(doc.allocate_attribute("normalmap", ""));
				appearance->append_attribute(doc.allocate_attribute("displacementmap", ""));
				appearance->append_attribute(doc.allocate_attribute("specularmap", ""));
				defaultSphereObject->append_node(appearance);
			}

			//xml_node<>* defaultPlaneObject = doc.allocate_node(node_element, "Object");
			//defaultPlaneObject->append_attribute(doc.allocate_attribute("name", "Plane"));
			//defaultPlaneObject->append_attribute(doc.allocate_attribute("IsFlatPlane", "1"));
			//defaultPlaneObject->append_attribute(doc.allocate_attribute("IsTerrain", "0"));
			//defaultElements->append_node(defaultPlaneObject);

			//// Plane Details
			//{

			//}
		}
	}

	newDefaultScenesConfig << doc;

	newDefaultScenesConfig.close();
	doc.clear();
}
