#include "Scene.h"

#include "../DX11Engine.h"
#include "../ApplicationNew.h"

Scene::Scene(Player& player) : m_Player(&player)
{
	_planeVertexBuffer = nullptr;
	_planeIndexBuffer = nullptr;
}

Scene::~Scene()
{
	Scene::Cleanup();
}

void Scene::Cleanup()
{
	_flatPlane->Cleanup();
	_flatPlane = nullptr;

	_planeVertexBuffer->Release();
	_planeIndexBuffer->Release();

	for (auto const sceneElement : _sceneElements)
	{
		if (sceneElement)
		{
			sceneElement->Cleanup();
		}
	}

	_sceneElements.clear();
}

void Scene::PreResizeViews()
{

}

void Scene::ResizeViews(float windowWidth, float windowHeight)
{
	m_Player->ResetPlayerCamera(windowWidth, windowHeight);
}

void Scene::InitialiseScene(float windowWidth, float windowHeight)
{
	auto texHandler = DX11Engine::Get().GetTextureHandler();
	ObjectMaterial aircraftMat{};
	aircraftMat.ambient = XMFLOAT4(1.000f, 0.766f, 0.336f, 1.0f);
	aircraftMat.diffuse = XMFLOAT4(1.000f, 0.766f, 0.336f, 1.0f);
	aircraftMat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);

	ObjectMaterial concrete{};
	concrete.ambient = XMFLOAT4(0.51f, 0.51f, 0.51f, 1.0f);
	concrete.diffuse = XMFLOAT4(0.51f, 0.51f, 0.51f, 1.0f);
	concrete.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f);

	ObjectMaterial shiny{};
	shiny.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.specular = XMFLOAT4(0.560f, 0.570f, 0.580f, 0.3f);

	ObjectMaterial charcoal{};
	charcoal.ambient = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
	charcoal.diffuse = XMFLOAT4(0.02f, 0.02f, 0.02f, 1.0f);
	charcoal.specular = XMFLOAT4(0.02f, 0.02f, 0.02f, 0.3f);

	ObjectMaterial matte{};
	matte.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	matte.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	matte.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.3f);

	_flatPlane = new FlatTerrain();
	_flatPlane->SetTerrainValues(50, 50, 64);
	_flatPlane->GenerateTerrain();

	BufferClass::CreateGroundPlane(&_planeVertexBuffer, &_planeIndexBuffer);

	ObjectMesh planeMesh;
	planeMesh.vertexBuffer = _flatPlane->GetVertexBuffer();
	planeMesh.indexBuffer = _flatPlane->GetIndexBuffer();
	planeMesh.numberOfIndices = static_cast<int>(_flatPlane->GetMeshData()->Indices.size());
	planeMesh.vertexBufferOffset = 0;
	planeMesh.vertexBufferStride = sizeof(SimpleVertex);

	NewObjectMesh sphere;
	ModelLoader::LoadModel(ApplicationNew::Get().GetDevice().Get(), L"Core/Resources/Objects/spherex.obj", sphere, false);

	auto groundTransform = std::make_shared<Transform>();
	groundTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	groundTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	groundTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto groundAppearance = std::make_shared<Appearance>(planeMesh, concrete);
	groundAppearance->SetColourTexture(texHandler->GetTextureByName("ConcreteColour"));
	groundAppearance->SetNormalMap(texHandler->GetTextureByName("ConcreteNormal"));
	//groundAppearance->SetDisplacementMap(_textureHandler->GetStoneDisplacementMap());
	//groundAppearance->SetSpecularMap(_textureHandler->GetGroundSpecularMap());

	auto sphereTransform = std::make_shared<Transform>();
	sphereTransform->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
	sphereTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphereTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto sphereAppearance = std::make_shared<Appearance>(sphere, shiny);

	//Scene elements
	{
		auto* groundPlaneElement = new SceneElement(GetStringHash("Ground Plane"), *groundTransform, *groundAppearance);
		groundPlaneElement->SetCastShadows(false);
		groundPlaneElement->SetAffectedByLight(true);
		//_flatPlane->SetGameObject(groundPlaneElement);
		_sceneElements.push_back(groundPlaneElement);

		auto* sphereElement = new SceneElement(GetStringHash("Sphere"), *sphereTransform, *sphereAppearance);
		sphereElement->SetCastShadows(true);
		sphereElement->SetAffectedByLight(true);
		_sceneElements.push_back(sphereElement);
	}
}

void Scene::Update(UpdateEvent& e)
{
	for (auto element : _sceneElements)
	{
		element->Update(e.ElapsedTime);

		if (element->GetElementName() == GetStringHash("Sphere"))
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
		}
	}
}

void Scene::Draw()
{
}
