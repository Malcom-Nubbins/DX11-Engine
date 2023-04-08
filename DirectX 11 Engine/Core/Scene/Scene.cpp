#include "Scene.h"

#include "../DX11Engine.h"
#include "../ApplicationNew.h"
#include "../Core/Loaders/ConfigLoader.h"
#include <codecvt>

Scene::Scene(const SceneInfo& SceneInfo) : m_SceneInfo(SceneInfo)
{
}

Scene::~Scene()
{
	Scene::Cleanup();
}

void Scene::Cleanup()
{
	for (SceneElement* sceneElement : m_SceneElements)
	{
		sceneElement->Cleanup();
		delete sceneElement;
	}

	m_SceneElements.clear();
}

void Scene::PreResizeViews()
{

}

void Scene::ResizeViews(float windowWidth, float windowHeight)
{
}

void Scene::Unload()
{
	m_bIsUnloading = true;
}

void Scene::InitialiseScene()
{
	using namespace std;

	auto texHandler = DX11Engine::Get().GetTextureHandler();
	auto graphicsHandler = DX11Engine::Get().GetGraphicsHandler();

	const vector<SceneElementInfo>& ElementInfos = m_SceneInfo.ElementInfos;

	for (const SceneElementInfo& ElementInfo : ElementInfos)
	{
		ObjectMaterial Material = graphicsHandler->GetMaterialByName(ElementInfo.MaterialName.c_str());
		shared_ptr<Appearance> objectAppearance;

		if (!ElementInfo.ModelName.empty())
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring modelPath(converter.from_bytes(OBJECTS_DIR));
			std::wstring modelFN(converter.from_bytes(ElementInfo.ModelName));

			wstring fullModelPath(modelPath + modelPath);

			NewObjectMesh objectMesh;
			ModelLoader::LoadModel(ApplicationNew::Get().GetDevice().Get(), fullModelPath, objectMesh, false);

			objectAppearance = make_shared<Appearance>(objectMesh, Material);
		}
		else
		{
			if (ElementInfo.bIsFlatPlane)
			{
				FlatTerrain* flatPlane = new FlatTerrain();
				flatPlane->SetTerrainValues(ElementInfo.TerrainGenWidth, ElementInfo.TerrainGenHeight, ElementInfo.TerrainGenSize);
				flatPlane->GenerateTerrain();

				ObjectMesh planeMesh;
				planeMesh.VertexBuffer = flatPlane->GetVertexBuffer();
				planeMesh.IndexBuffer = flatPlane->GetIndexBuffer();
				planeMesh.NumberOfIndices = static_cast<int>(flatPlane->GetMeshData()->Indices.size());
				planeMesh.VertexBufferOffset = 0;
				planeMesh.vertexBufferStride = sizeof(SimpleVertex);

				objectAppearance = make_shared<Appearance>(planeMesh, Material);

				flatPlane->Cleanup();
				flatPlane = nullptr;
			}
			else if (ElementInfo.bIsTerrain)
			{
				DiamondSquareTerrain* terrain = new DiamondSquareTerrain();
				terrain->SetTerrainValues(ElementInfo.TerrainGenWidth, ElementInfo.TerrainGenHeight, ElementInfo.TerrainGenSize);
				terrain->GenerateTerrain();

				ObjectMesh terrainMesh;
				terrainMesh.VertexBuffer = terrain->GetVertexBuffer();
				terrainMesh.IndexBuffer = terrain->GetIndexBuffer();
				terrainMesh.NumberOfIndices = static_cast<int>(terrain->GetMeshData()->Indices.size());
				terrainMesh.VertexBufferOffset = 0;
				terrainMesh.vertexBufferStride = sizeof(SimpleVertex);

				objectAppearance = make_shared<Appearance>(terrainMesh, Material);

				terrain->Cleanup();
				terrain = nullptr;
			}
		}

		if (!ElementInfo.TextureName.empty())
		{
			objectAppearance->SetColourTexture(texHandler->GetTextureByName(ElementInfo.TextureName.c_str()));
		}

		if (!ElementInfo.NormalMapName.empty())
		{
			objectAppearance->SetNormalMap(texHandler->GetTextureByName(ElementInfo.NormalMapName.c_str()));
		}

		if (!ElementInfo.DisplacementMapName.empty())
		{
			objectAppearance->SetDisplacementMap(texHandler->GetTextureByName(ElementInfo.DisplacementMapName.c_str()));
		}

		if (!ElementInfo.SpecularMapName.empty())
		{
			objectAppearance->SetSpecularMap(texHandler->GetTextureByName(ElementInfo.SpecularMapName.c_str()));
		}

		shared_ptr<Transform> objectTransform = make_shared<Transform>();
		objectTransform->SetPosition(ElementInfo.TransformPosition);
		objectTransform->SetScale(ElementInfo.TranformScale);
		objectTransform->SetRotation(ElementInfo.TransformRotation);

		SceneElement* NewSceneElement = m_SceneElements.emplace_back(new SceneElement(GetStringHash(ElementInfo.ObjectName), *objectTransform, *objectAppearance));
		NewSceneElement->SetCastShadows(true);
		NewSceneElement->SetAffectedByLight(true);
	}
}

void Scene::Update(UpdateEvent& e)
{
	if (!m_bIsUnloading)
	{
		for (SceneElement* element : m_SceneElements)
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
}

void Scene::Draw()
{
}
