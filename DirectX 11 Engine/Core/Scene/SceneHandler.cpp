#include "SceneHandler.h"

#include "../ApplicationNew.h"
#include "../Core/Loaders/ConfigLoader.h"
#include "../Core/Loaders/XMLLoader/rapidxml.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_ext.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_print.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_utils.hpp"
#include <codecvt>

SceneHandler::SceneHandler()
	: IConfigInterface(SCENES_CONFIG)
	, m_CurrentScene(nullptr)
	, m_CurrSceneIndex(0)
{
	LoadConfig();
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

	m_SceneInfos.clear();
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

void SceneHandler::LoadScenesFromSceneInfo(const SceneInfo& SceneToLoad)
{	
	if (m_CurrentScene != nullptr)
	{
		// Unload and nullify
		m_CurrentScene->Unload();
		m_CurrentScene->Cleanup();

		m_CurrentScene = nullptr;
	}

	m_CurrentScene = new Scene(SceneToLoad);
	m_CurrentScene->InitialiseScene();
}

void SceneHandler::GoToNextScene()
{
	++m_CurrSceneIndex;

	if (m_CurrSceneIndex > m_SceneInfos.size() - 1)
	{
		m_CurrSceneIndex = 0;
	}

	LoadScenesFromSceneInfo(m_SceneInfos[m_CurrSceneIndex]);
}

void SceneHandler::GoToPreviousScene()
{
	--m_CurrSceneIndex;

	if (m_CurrSceneIndex < 0)
	{
		m_CurrSceneIndex = static_cast<int>(m_SceneInfos.size()) - 1;
	}

	LoadScenesFromSceneInfo(m_SceneInfos[m_CurrSceneIndex]);
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

void SceneHandler::CreateConfig()
{
	using namespace rapidxml;
	using namespace std;

	ofstream newDefaultScenesConfig(GetConfigName());

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

void SceneHandler::LoadConfig()
{
	using namespace rapidxml;
	using namespace std;

	if (!DoesConfigExist())
	{
		CreateConfig();
	}

	file<> xmlFile(SCENES_CONFIG);
	xml_document<> doc;

	doc.parse<0>(xmlFile.data());

	xml_node<>* rootNode = doc.first_node("Scenes");

	for (xml_node<>* sceneNode = rootNode->first_node("Scene"); sceneNode; sceneNode = sceneNode->next_sibling())
	{
		std::string sceneName(sceneNode->first_attribute("name")->value());

		SceneInfo NewSceneInfo(sceneName);
		vector<SceneElementInfo> NewElementInfos;

		xml_node<>* elements = sceneNode->first_node("Elements");

		for (xml_node<>* object = elements->first_node("Object"); object; object = object->next_sibling())
		{
			std::string objectName(object->first_attribute("name")->value());
			int const numOfObject = stoi(object->first_attribute("Amount")->value());

			bool const isFlatPlane = stoi(object->first_attribute("IsFlatPlane")->value());
			bool const isTerrain = stoi(object->first_attribute("IsTerrain")->value());
			int const generatorWidth = stoi(object->first_attribute("GenWidth")->value());
			int const generatorHeight = stoi(object->first_attribute("GenHeight")->value());
			int const generatorSize = stoi(object->first_attribute("GenSize")->value());

			xml_node<>* appearance = object->first_node("Appearance");
			std::string const materialName(appearance->first_attribute("material")->value());
			std::string const modelName(appearance->first_attribute("model")->value());
			std::string const textureName(appearance->first_attribute("texture")->value());
			std::string const normalMapName(appearance->first_attribute("normalmap")->value());
			std::string const displacementMapName(appearance->first_attribute("displacementmap")->value());
			std::string const specularMapName(appearance->first_attribute("specularmap")->value());

			xml_node<>* transform = object->first_node("Transform");
			xml_node<>* positionNode = transform->first_node("Position");
			xml_node<>* scaleNode = transform->first_node("Scale");
			xml_node<>* rotationNode = transform->first_node("Rotation");

			const XMFLOAT3 scale(strtof(scaleNode->first_attribute("x")->value(), nullptr),
				strtof(scaleNode->first_attribute("y")->value(), nullptr),
				strtof(scaleNode->first_attribute("z")->value(), nullptr));

			const XMFLOAT3 rotation(strtof(rotationNode->first_attribute("x")->value(), nullptr),
				strtof(rotationNode->first_attribute("y")->value(), nullptr),
				strtof(rotationNode->first_attribute("z")->value(), nullptr));

			std::string const xPos(positionNode->first_attribute("x")->value());
			std::string const yPos(positionNode->first_attribute("y")->value());
			std::string const zPos(positionNode->first_attribute("z")->value());

			TStringHash const randPosStr(GetStringHash("rand"));

			for (uint32 idx = 0; idx < static_cast<uint32>(numOfObject); ++idx)
			{
				float finalXPos = (GetStringHash(xPos) == randPosStr) ? MathsHandler::RandomFloat(-50, 50) : strtof(xPos.c_str(), nullptr);
				float finalYPos = strtof(yPos.c_str(), nullptr);
				float finalZPos = (GetStringHash(zPos) == randPosStr) ? MathsHandler::RandomFloat(-50, 50) : strtof(zPos.c_str(), nullptr);

				const XMFLOAT3 Position(finalXPos, finalYPos, finalZPos);

				std::string finalObjName = objectName + " %d";
				finalObjName = FormatCString(finalObjName.c_str(), idx);

				NewElementInfos.emplace_back(finalObjName, numOfObject,
					isFlatPlane, isTerrain, generatorWidth, generatorHeight, generatorSize,
					Position, scale, rotation,
					modelName, materialName, textureName, normalMapName, displacementMapName, specularMapName);
			}
		}

		NewSceneInfo.ElementInfos = NewElementInfos;
		m_SceneInfos.push_back(move(NewSceneInfo));
	}

	if (m_SceneInfos.size() > 0)
	{
		LoadScenesFromSceneInfo(m_SceneInfos.front());
	}
}