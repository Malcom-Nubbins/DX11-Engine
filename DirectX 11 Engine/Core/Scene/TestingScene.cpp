#include "TestingScene.h"
#include "../Loaders/ModelLoader.h"
#include "../ApplicationNew.h"

TestingScene::TestingScene(Player& player)
	: Scene(player)
{
	_shadows = nullptr;
	_skyGradient = nullptr;
	_renderToQuad = nullptr;
	_planeVertexBuffer = nullptr;
	_planeIndexBuffer = nullptr;

	_buttonCooldown = 1.0f;
	_currentCooldown = 0.0f;
}

TestingScene::~TestingScene()
{
}

void TestingScene::InitialiseScene(float windowWidth, float windowHeight)
{
	Scene::InitialiseScene(windowWidth, windowHeight);

	InitialiseSceneGraphics(windowWidth, windowHeight);

	_sceneLight.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_sceneLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.lightDirection = XMFLOAT3(5.0f, 5.0f, 0.0f);

	_spotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_spotLight.diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_spotLight.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	_spotLight.attenuation = XMFLOAT3(0.4f, 0.02f, 0.0f);
	_spotLight.spot = 20.0f;
	_spotLight.range = 1000.0f;

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
	groundAppearance->SetColourTexture(TextureHandler::GetTextureByName("ConcreteColour"));
	groundAppearance->SetNormalMap(TextureHandler::GetTextureByName("ConcreteNormal"));
	//groundAppearance->SetDisplacementMap(_textureHandler->GetStoneDisplacementMap());
	//groundAppearance->SetSpecularMap(_textureHandler->GetGroundSpecularMap());

	auto sphereTransform = std::make_shared<Transform>();
	sphereTransform->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
	sphereTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	sphereTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto sphereAppearance = std::make_shared<Appearance>(sphere, shiny);

	//Scene elements
	{
		auto* groundPlaneElement = new SceneElement("Ground Plane", *groundTransform, *groundAppearance);
		groundPlaneElement->SetCastShadows(false);
		groundPlaneElement->SetAffectedByLight(true);
		//_flatPlane->SetGameObject(groundPlaneElement);
		_sceneElements.push_back(groundPlaneElement);

		auto* sphereElement = new SceneElement("Sphere", *sphereTransform, *sphereAppearance);
		sphereElement->SetCastShadows(true);
		sphereElement->SetAffectedByLight(true);
		_sceneElements.push_back(sphereElement);
	}
}

void TestingScene::InitialiseSceneGraphics(float windowWidth, float windowHeight)
{
	_basicLight = new BasicLight();
	_basicLight->Initialise(windowWidth, windowHeight);

	_shadows = new Shadows();
	_shadows->Initialise(4096, 4096);

	_skyGradient = new SkyColourGradient();
	_skyGradient->Initialise();

	_renderToQuad = new RenderToFullscreenQuad();
	_renderToQuad->Initialise(windowWidth, windowHeight);
}

void TestingScene::Cleanup()
{
	_skyGradient->Cleanup();
	_skyGradient = nullptr;

	_shadows->Cleanup();
	_shadows = nullptr;

	_basicLight->Cleanup();
	_basicLight = nullptr;

	_renderToQuad->Cleanup();
	_renderToQuad = nullptr;

	/*_flatPlane->Cleanup();
	_flatPlane = nullptr;*/

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

void TestingScene::PreResizeViews()
{
	Scene::PreResizeViews();
	_basicLight->PreResize();
	_shadows->PreResizeViews();
}

void TestingScene::ResizeViews(float windowWidth, float windowHeight)
{
	Scene::ResizeViews(windowWidth, windowHeight);
	_basicLight->Resize(windowWidth, windowHeight);
	_shadows->OnResize(4096, 4096);
	m_Player->ResetPlayerCamera(windowWidth, windowHeight);
}

void TestingScene::Update(UpdateEvent& e)
{
	Scene::Update(e);

	/*_spotLight.position = _player.GetPlayerPosition();
	_spotLight.direction = _player.GetPlayerLookDirection();*/

	_shadows->UpdateLightDirection(_sceneLight.lightDirection);
	_shadows->SetSceneCentre(m_Player->GetPlayerPosition());
	_shadows->BuildShadowTransform();

	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('R'))
	{
		if (_currentCooldown <= 0.0f)
		{
			_basicLight->SetWireframeMode(!_basicLight->GetWireframeState());
			_currentCooldown = _buttonCooldown;
		}
	}

	_skyGradient->SetSceneCentre(m_Player->GetPlayerPosition());
	_skyGradient->Update(e.ElapsedTime);

	for (auto element : _sceneElements)
	{
		element->Update(e.ElapsedTime);
	}

	if (_currentCooldown > 0.0f)
		_currentCooldown -= e.ElapsedTime;
}

void TestingScene::Draw()
{
	_basicLight->SetAsCurrentRenderTarget();
	_basicLight->SetAsCurrentViewport();
	RenderClass::DisableRtvClearing();

	XMFLOAT3 sunPos(0.0f, 50.0f, 0.0f);

	_skyGradient->Render(m_Player->GetCamera(), sunPos);

	RenderClass::EnableZBuffer();
	_shadows->Render(_sceneElements);

	_basicLight->Render(m_Player->GetCamera(), _sceneLight, _spotLight, _sceneElements, *_shadows);

	_renderToQuad->Render(_basicLight->GetRenderTargetSRV());
}
