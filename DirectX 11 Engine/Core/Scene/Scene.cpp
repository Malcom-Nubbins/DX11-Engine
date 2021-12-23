#include "Scene.h"

#include "../DX11Engine.h"
#include "../ApplicationNew.h"
#include <codecvt>

Scene::Scene(char const* sceneName) : m_SceneName(sceneName)
{
}

Scene::~Scene()
{
	Scene::Cleanup();
}

void Scene::Cleanup()
{
	for (auto const sceneElement : m_SceneElements)
	{
		if (sceneElement)
		{
			sceneElement->Cleanup();
		}
	}

	m_SceneElements.clear();
}

void Scene::PreResizeViews()
{

}

void Scene::ResizeViews(float windowWidth, float windowHeight)
{
}

void Scene::InitialiseScene(rapidxml::xml_document<>& doc, rapidxml::xml_node<>& sceneNode)
{
	using namespace std;
	using namespace rapidxml;

	auto texHandler = DX11Engine::Get().GetTextureHandler();
	auto graphicsHandler = DX11Engine::Get().GetGraphicsHandler();

	xml_node<>* elements = sceneNode.first_node("Elements");

	for (xml_node<>* object = elements->first_node("Object"); object; object = object->next_sibling())
	{
		std::string objectName(object->first_attribute("name")->value());
		int const numOfObject = stoi(object->first_attribute("Amount")->value());
		bool const isFlatPlane = stoi(object->first_attribute("IsFlatPlane")->value());
		bool const isTerrain = stoi(object->first_attribute("IsTerrain")->value());

		xml_node<>* appearance = object->first_node("Appearance");

		std::string const materialName(appearance->first_attribute("material")->value());
		std::string const modelName(appearance->first_attribute("model")->value());
		std::string const textureName(appearance->first_attribute("texture")->value());
		std::string const normalMapName(appearance->first_attribute("normalmap")->value());
		std::string const displacementMapName(appearance->first_attribute("displacementmap")->value());
		std::string const specularMapName(appearance->first_attribute("specularmap")->value());

		ObjectMaterial mat = graphicsHandler->GetMaterialByName(materialName.c_str());

		std::shared_ptr<Appearance> objectAppearance;

		if (!modelName.empty())
		{
			wstring modelFilePath = L"Core/Resources/Objects/";

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring modelPath(converter.from_bytes(modelName));

			wstring fullModelPath(modelFilePath + modelPath);

			NewObjectMesh objectMesh;
			ModelLoader::LoadModel(ApplicationNew::Get().GetDevice().Get(), fullModelPath, objectMesh, false);
		
			objectAppearance = std::make_shared<Appearance>(objectMesh, mat);
		}
		else
		{
			int const generatorWidth = stoi(object->first_attribute("GenWidth")->value());
			int const generatorHeight = stoi(object->first_attribute("GenHeight")->value());
			int const generatorSize = stoi(object->first_attribute("GenSize")->value());

			if (isFlatPlane)
			{
				FlatTerrain* flatPlane = new FlatTerrain();
				flatPlane->SetTerrainValues(generatorWidth, generatorHeight, generatorSize);
				flatPlane->GenerateTerrain();

				ObjectMesh planeMesh;
				planeMesh.vertexBuffer = flatPlane->GetVertexBuffer();
				planeMesh.indexBuffer = flatPlane->GetIndexBuffer();
				planeMesh.numberOfIndices = static_cast<int>(flatPlane->GetMeshData()->Indices.size());
				planeMesh.vertexBufferOffset = 0;
				planeMesh.vertexBufferStride = sizeof(SimpleVertex);

				objectAppearance = std::make_shared<Appearance>(planeMesh, mat);
				
				flatPlane->Cleanup();
				flatPlane = nullptr;
			}
			else if (isTerrain)
			{
				DiamondSquareTerrain* terrain = new DiamondSquareTerrain();
				terrain->SetTerrainValues(generatorWidth, generatorHeight, generatorSize);
				terrain->GenerateTerrain();

				ObjectMesh terrainMesh;
				terrainMesh.vertexBuffer = terrain->GetVertexBuffer();
				terrainMesh.indexBuffer = terrain->GetIndexBuffer();
				terrainMesh.numberOfIndices = static_cast<int>(terrain->GetMeshData()->Indices.size());
				terrainMesh.vertexBufferOffset = 0;
				terrainMesh.vertexBufferStride = sizeof(SimpleVertex);

				objectAppearance = std::make_shared<Appearance>(terrainMesh, mat);

				terrain->Cleanup();
				terrain = nullptr;
			}
		}

		if (!textureName.empty())
		{
			objectAppearance->SetColourTexture(texHandler->GetTextureByName(textureName.c_str()));
		}

		if (!normalMapName.empty())
		{
			objectAppearance->SetNormalMap(texHandler->GetTextureByName(normalMapName.c_str()));
		}

		if (!displacementMapName.empty())
		{
			objectAppearance->SetDisplacementMap(texHandler->GetTextureByName(displacementMapName.c_str()));
		}

		if (!specularMapName.empty())
		{
			objectAppearance->SetSpecularMap(texHandler->GetTextureByName(specularMapName.c_str()));
		}

		xml_node<>* transform = object->first_node("Transform");

		xml_node<>* positionNode = transform->first_node("Position");
		xml_node<>* scaleNode = transform->first_node("Scale");
		xml_node<>* rotationNode = transform->first_node("Rotation");

		XMFLOAT3 scale(strtof(scaleNode->first_attribute("x")->value(), nullptr),
			strtof(scaleNode->first_attribute("y")->value(), nullptr),
			strtof(scaleNode->first_attribute("z")->value(), nullptr));

		XMFLOAT3 rotation(strtof(rotationNode->first_attribute("x")->value(), nullptr),
			strtof(rotationNode->first_attribute("y")->value(), nullptr),
			strtof(rotationNode->first_attribute("z")->value(), nullptr));

		std::string const xPos(positionNode->first_attribute("x")->value());
		std::string const yPos(positionNode->first_attribute("y")->value());
		std::string const zPos(positionNode->first_attribute("z")->value());

		StringHash const randPosStr(GetStringHash("rand"));

		for (u32 idx = 0; idx < numOfObject; ++idx)
		{
			float finalXPos = (GetStringHash(xPos) == randPosStr) ? MathsHandler::RandomFloat(-50, 50) : strtof(xPos.c_str(), nullptr);
			float finalYPos = strtof(yPos.c_str(), nullptr);
			float finalZPos = (GetStringHash(zPos) == randPosStr) ? MathsHandler::RandomFloat(-50, 50) : strtof(zPos.c_str(), nullptr);

			XMFLOAT3 position(finalXPos, finalYPos, finalZPos);

			auto objectTransform = std::make_shared<Transform>();
			objectTransform->SetPosition(position);
			objectTransform->SetScale(scale);
			objectTransform->SetRotation(rotation);

			std::string finalObjName = objectName + " %d";
			finalObjName = FormatCString(finalObjName.c_str(), idx);

			SceneElement* newSceneElement = new SceneElement(GetStringHash(finalObjName), *objectTransform, *objectAppearance);
			newSceneElement->SetCastShadows(true);
			newSceneElement->SetAffectedByLight(true);

			m_SceneElements.emplace_back(std::move(newSceneElement));
		}
	}
}

void Scene::Update(UpdateEvent& e)
{
	for (auto element : m_SceneElements)
	{
		element->Update(e.ElapsedTime);

		/*if (element->GetElementName() == GetStringHash("Sphere"))
		{
			DX11Engine const& engine = DX11Engine::Get();
			XMFLOAT2 screenSize(engine.GetClientWidth(), engine.GetClientHeight());
			XMFLOAT2 screenPos(0.0f, 0.0f);
			bool isOnScreen = MathsHandler::WorldToScreen(element->GetTransform()->GetPosition(),
				screenPos,
				element->GetTransform()->GetWorld(), m_Player->GetCamera().GetView(), m_Player->GetCamera().GetPerspectiveProj(),
				screenSize);

			UIBitmap* uiElement = engine.GetUI()->GetUIElement(GetStringHash("GrassyTest"));
			if (uiElement != nullptr)
			{
				uiElement->SetShouldDraw(isOnScreen);

				if (isOnScreen)
				{
					uiElement->SetDynamicPos(screenPos);
				}
			}
		}*/
	}
}

void Scene::Draw()
{
}
