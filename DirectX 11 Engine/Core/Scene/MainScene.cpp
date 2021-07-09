#include "MainScene.h"
#include "../ApplicationNew.h"

MainScene::MainScene(Player& player)
	: Scene(player)
	, _sceneLight()
	, _sceneFog()
	, _basicLight(nullptr)
	, _spotLight()
{
	_shadows = nullptr;
	_skyGradient = nullptr;
	_renderToQuad = nullptr;
	_heatHaze = nullptr;
	_planeVertexBuffer = nullptr;
	_planeIndexBuffer = nullptr;

	_lightRotationAmount = XMConvertToRadians(00.0f);

	_fullRotationAmount = XMConvertToRadians(360.0f);
	_currentTime = XMConvertToRadians(90.0f);
	_currentDay = 1;
	_currentSeason = 0;

	_seasonsSunHeightOffset = std::vector<int>
	{
		{0},
		{2},
		{0},
		{-2}
	};

	_seasonNames = std::map<int, std::string>
	{
		{0, "Spring"},
		{1, "Summer"},
		{2, "Autumn"},
		{3, "Winter"}
	};

	_diamondSquareTerrain = nullptr;
	_buttonCooldown = 1.0f;
	_currentCooldown = 0.0f;
}

MainScene::~MainScene()
{
}

void MainScene::Cleanup()
{
	_heatHaze->Cleanup();
	delete _heatHaze;
	_heatHaze = nullptr;

	_skyGradient->Cleanup();
	delete _skyGradient;
	_skyGradient = nullptr;

	_shadows->Cleanup();
	delete _shadows;
	_shadows = nullptr;

	_basicLight->Cleanup();
	delete _basicLight;
	_basicLight = nullptr;

	_renderToQuad->Cleanup();
	delete _renderToQuad;
	_renderToQuad = nullptr;

	_planeVertexBuffer->Release();
	_planeIndexBuffer->Release();

	_sceneElements.shrink_to_fit();
}

void MainScene::PreResizeViews()
{
	Scene::PreResizeViews();
	_basicLight->PreResize();
}

void MainScene::ResizeViews(float windowWidth, float windowHeight)
{
	Scene::ResizeViews(windowWidth, windowHeight);
	//_d3dClass->GetSwapChain()->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	_basicLight->Resize(windowWidth, windowHeight);
	_heatHaze->Resize(windowWidth, windowHeight);
}

void MainScene::InitialiseScene(float windowWidth, float windowHeight)
{
	Scene::InitialiseScene(windowWidth, windowHeight);

	InitialiseSceneGraphics(windowWidth, windowHeight);

	_sceneLight.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_sceneLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.lightDirection = XMFLOAT3(5.0f, 0.0f, 0.0f);

	_spotLight.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_spotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_spotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_spotLight.attenuation = XMFLOAT3(0.4f, 0.02f, 0.0f);
	_spotLight.spot = 20.0f;
	_spotLight.range = 1000.0f;

	_preOffsetLightDir = _sceneLight.lightDirection;

	_sceneFog.FogStart = 5.0f;
	_sceneFog.FogRange = 50.0f;
	_sceneFog.FogColourDay = XMFLOAT4(0.7f, 0.80f, 0.92f, 1.0f);
	_sceneFog.FogColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	_sceneFog.FogColourSunriseSunset = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f);
	_sceneFog.UseFog = 1.0f;
	
	ObjectMaterial aircraftMat;
	aircraftMat.ambient = XMFLOAT4(1.000, 0.766, 0.336, 1.0f);
	aircraftMat.diffuse = XMFLOAT4(1.000, 0.766, 0.336, 1.0f);
	aircraftMat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);

	ObjectMaterial concrete;
	concrete.ambient = XMFLOAT4(0.51, 0.51, 0.51, 1.0f);
	concrete.diffuse = XMFLOAT4(0.51, 0.51, 0.51, 1.0f);
	concrete.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f);

	ObjectMaterial shiny;
	shiny.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.specular = XMFLOAT4(0.560f, 0.570f, 0.580f, 0.3f);

	ObjectMaterial charcoal;
	charcoal.ambient = XMFLOAT4(0.02, 0.02, 0.02, 1.0f);
	charcoal.diffuse = XMFLOAT4(0.02, 0.02, 0.02, 1.0f);
	charcoal.specular = XMFLOAT4(0.02, 0.02, 0.02, 0.3f);

	ObjectMaterial matte;
	matte.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	matte.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	matte.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.3f);

	BufferClass::CreateGroundPlane(&_planeVertexBuffer, &_planeIndexBuffer);

	ObjectMesh planeMesh;
	planeMesh.vertexBuffer = _planeVertexBuffer;
	planeMesh.indexBuffer = _planeIndexBuffer;
	planeMesh.numberOfIndices = 6;
	planeMesh.vertexBufferOffset = 0;
	planeMesh.vertexBufferStride = sizeof(SimpleVertex);

	//ObjectMesh aircraftMesh = OBJLoader::Load(L"Core/Resources/Objects/Hercules.obj", _d3dClass->GetDevice(), false);
	//ObjectMesh sphere = OBJLoader::Load(L"Core/Resources/Objects/spherex.obj", _d3dClass->GetDevice(), false);
	//ObjectMesh plant0 = OBJLoader::Load(L"Core/Resources/Objects/plant0.obj", _d3dClass->GetDevice());
	//ObjectMesh plant1 = OBJLoader::Load(L"Core/Resources/Objects/plant1.obj", _d3dClass->GetDevice());

	auto device = ApplicationNew::Get().GetDevice();

	NewObjectMesh aircraftMesh, sphere, plant0, plant1;
	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/Hercules.obj", aircraftMesh, false);
	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/spherex.obj", sphere, false);
	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/plant0.obj", plant0, true);
	ModelLoader::LoadModel(device.Get(), L"Core/Resources/Objects/plant1.obj", plant1, true);

	_diamondSquareTerrain = new DiamondSquareTerrain();
	_diamondSquareTerrain->SetTerrainValues(256, 256, 512);
	_diamondSquareTerrain->GenerateTerrain();

	m_Player->SetPlayerPosition(XMFLOAT3(0.0f, _diamondSquareTerrain->GetHeight(0.0f, -2.0f) + 2.0f, -2.0f));

	ObjectMesh diamondSquareMesh;
	diamondSquareMesh.vertexBuffer = _diamondSquareTerrain->GetVertexBuffer();
	diamondSquareMesh.indexBuffer = _diamondSquareTerrain->GetIndexBuffer();
	diamondSquareMesh.numberOfIndices = static_cast<int>(_diamondSquareTerrain->GetMeshData()->Indices.size());
	diamondSquareMesh.vertexBufferOffset = 0;
	diamondSquareMesh.vertexBufferStride = sizeof(SimpleVertex);

	// Transforms
	auto* groundTransform = new Transform();
	groundTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	groundTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	groundTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto underworldTransform = new Transform();
	underworldTransform->SetPosition(XMFLOAT3(0.0f, -15.0f, 0.0f));
	underworldTransform->SetScale(XMFLOAT3(256.0f, 1.0f, 256.0f));
	underworldTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto* aircraftTransform = new Transform();
	aircraftTransform->SetPosition(XMFLOAT3(0.0f, _diamondSquareTerrain->GetHeight(0, 0) + 8.0f, 0));
	aircraftTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	aircraftTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto* sunSphereTransform = new Transform();
	sunSphereTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	sunSphereTransform->SetScale(XMFLOAT3(4.0f, 4.0f, 4.0f));
	sunSphereTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));


	// Appearances
	Appearance* groundAppearance = new Appearance(diamondSquareMesh, concrete);
	groundAppearance->SetColourTexture(TextureHandler::GetTextureByName("StoneColour"));
	groundAppearance->SetNormalMap(TextureHandler::GetTextureByName("StoneNormal"));
	groundAppearance->SetDisplacementMap(TextureHandler::GetTextureByName("StoneDisplacement"));
	//groundAppearance->SetSpecularMap(_systemHandlers->GetTextureHandler()->GetGroundSpecularMap());

	Appearance* underworldAppearance = new Appearance(planeMesh, shiny);
	underworldAppearance->SetColourTexture(TextureHandler::GetTextureByName("StoneColour"));
	underworldAppearance->SetNormalMap(TextureHandler::GetTextureByName("StoneNormal"));
	underworldAppearance->SetDisplacementMap(TextureHandler::GetTextureByName("StoneDisplacement"));

	Appearance* sphereAppearance = new Appearance(sphere, charcoal);
	//sphereAppearance->SetColourTexture(_systemHandlers->GetTextureHandler()->GetStoneTexture());
	//sphereAppearance->SetNormalMap(_systemHandlers->GetTextureHandler()->GetStoneNormalMap());
	//sphereAppearance->SetDisplacementMap(_systemHandlers->GetTextureHandler()->GetStoneDisplacementMap());

	Appearance* aircraftAppearance = new Appearance(aircraftMesh, aircraftMat);
	aircraftAppearance->SetColourTexture(TextureHandler::GetTextureByName("AircraftColour"));
	aircraftAppearance->SetNormalMap(TextureHandler::GetTextureByName("AircraftNormal"));
	//aircraftAppearance->SetSpecularMap(_systemHandlers->GetTextureHandler()->GetAircraftSpecularMap());
	//aircraftAppearance->SetDisplacementMap(_systemHandlers->GetTextureHandler()->GetAircraftDisplacementMap());

	Appearance* sunSphereAppearance = new Appearance(sphere, shiny);

	// Scene Elements
	{
		SceneElement* element = new SceneElement("Ground Plane", groundTransform, groundAppearance);
		element->SetCastShadows(true);
		element->SetAffectedByLight(true);
		_diamondSquareTerrain->SetGameObject(element);
		_diamondSquareTerrain->SetTerrainFinishedBuilding(true);
		_sceneElements.push_back(element);

		element = new SceneElement("Underworld", underworldTransform, underworldAppearance);
		element->SetCastShadows(true);
		element->SetAffectedByLight(true);
		_sceneElements.push_back(element);

		srand(time(NULL));
		for (int i = 0; i < 90; ++i)
		{
			Transform* sphereTransform = new Transform();
			sphereTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
			sphereTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
			sphereTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

			element = new SceneElement("Spheres", sphereTransform, sphereAppearance);
			const float randomX = MathsHandler::RandomFloat(-128, 128);
			const float randomZ = MathsHandler::RandomFloat(-128, 128);

			element->GetTransform()->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ) + 2.0f, randomZ));
			element->SetCastShadows(true);
			element->SetAffectedByLight(true);

			_sceneElements.push_back(element);
		}

		element = new SceneElement("Aircraft", aircraftTransform, aircraftAppearance);
		element->SetCastShadows(true);
		element->SetAffectedByLight(true);
		_sceneElements.push_back(element);

		PointLight light;
		light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
		light.diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		light.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
		light.attenuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
		light.range = 25.0f;
		light.position = m_Player->GetPlayerPosition();
		//_pointLights.push_back(light);


		auto* plantAppearance = new Appearance(plant0, matte);
		auto* plant2Appearance = new Appearance(plant1, matte);

		for (int i = 0; i < 60; ++i)
		{
			auto* plantTransform = new Transform();
			plantTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
			plantTransform->SetScale(XMFLOAT3(5.0f, 5.0f, 5.0f));
			plantTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

			float randomX = MathsHandler::RandomFloat(-90, 90);
			float randomZ = MathsHandler::RandomFloat(-90, 90);

			element = new SceneElement("Plant " + i, plantTransform, plantAppearance);
			element->GetTransform()->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ), randomZ));
			element->GetTransform()->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
			element->GetTransform()->SetRotation(XMFLOAT3(0.0f, XMConvertToRadians(rand()), 0.0f));
			element->SetCastShadows(true);
			element->SetAffectedByLight(true);

			/*light.Position = element->GetPosition();
			_pointLights.push_back(light);*/

			_sceneElements.push_back(element);

			randomX = MathsHandler::RandomFloat(-90, 90);
			randomZ = MathsHandler::RandomFloat(-90, 90);

			element = new SceneElement("Plant " + i, plantTransform, plant2Appearance);
			element->GetTransform()->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ), randomZ));
			element->GetTransform()->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
			element->GetTransform()->SetRotation(XMFLOAT3(0.0f, XMConvertToRadians(rand()), 0.0f));
			element->SetCastShadows(true);
			element->SetAffectedByLight(true);

			/*light.Position = element->GetPosition();
			_pointLights.push_back(light);*/

			_sceneElements.push_back(element);
		}

		element = new SceneElement("Light Source Sphere", sunSphereTransform, sunSphereAppearance);
		element->SetCastShadows(false);
		element->SetAffectedByLight(false);
		_sceneElements.push_back(element);
	}
}

void MainScene::InitialiseSceneGraphics(float windowWidth, float windowHeight)
{
	_basicLight = new BasicLight();
	_basicLight->Initialise(windowWidth, windowHeight);

	_shadows = new Shadows();
	_shadows->Initialise(8192, 8192);

	_skyGradient = new SkyColourGradient();
	_skyGradient->Initialise();

	_renderToQuad = new RenderToFullscreenQuad();
	_renderToQuad->Initialise(windowWidth, windowHeight);

	_heatHaze = new HeatHaze();
	_heatHaze->Initialise(windowWidth, windowHeight);
}

void MainScene::Update(float deltaTime)
{	
	Scene::Update(deltaTime);

	_spotLight.position = m_Player->GetPlayerPosition();
	_spotLight.direction = m_Player->GetPlayerLookDirection();
	//_pointLights.at(0).Position = _camera->GetPosition();
	

	XMFLOAT3 lightDir = _preOffsetLightDir;

	// Rotate the light at a 45 degree angle
	XMFLOAT3 rotationAxis = XMFLOAT3(0.0f, -0.5f, 0.5f);
	XMVECTOR axis = XMLoadFloat3(&rotationAxis);
	XMVECTOR rotationQuaternion = XMQuaternionRotationAxis(axis, _lightRotationAmount * deltaTime);
	
	XMVECTOR rotatedLight = XMVector3Rotate(XMLoadFloat3(&lightDir), rotationQuaternion);
	XMStoreFloat3(&lightDir, rotatedLight);
	_preOffsetLightDir = lightDir;

	_currentTime += _lightRotationAmount * deltaTime;
	if (_currentTime >= _fullRotationAmount)
	{
		// Day has passed
		_currentDay++;
		if (_currentDay > 16)
			_currentDay = 1;

		_currentTime = 0.0f;

		if (_currentDay % 4 == 0)
		{
			// Season has passed
			_currentSeason++;
			if (_currentSeason > 3)
				_currentSeason = 0;
		}
	}
	else if (_currentTime < 0.0f)
	{
		_currentDay--;
		if (_currentDay < 1)
			_currentDay = 16;
		_currentTime = _fullRotationAmount;

		if (_currentDay % 4 == 0)
		{
			// Season has passed
			_currentSeason--;
			if (_currentSeason < 0)
				_currentSeason = 3;
		}
	}

	// Raise or lower the height of the sun
	lightDir.y += _seasonsSunHeightOffset[_currentSeason];

	if (_seasonNames.find(_currentSeason)->second == "Spring" || _seasonNames.find(_currentSeason)->second == "Autumn")
	{
		_sceneFog.FogStart = 13.0f;
		_sceneFog.FogRange = 150.0f;
	}
	else if (_seasonNames.find(_currentSeason)->second == "Summer")
	{
		_sceneFog.FogStart = 24.0f;
		_sceneFog.FogRange = 240.0f;
	}
	else if (_seasonNames.find(_currentSeason)->second == "Winter")
	{
		_sceneFog.FogStart = 7.0f;
		_sceneFog.FogRange = 80.0f;
	}

	if (GetAsyncKeyState(VK_OEM_PERIOD))
	{
		_lightRotationAmount += 0.5f * deltaTime;

		if (_lightRotationAmount > 2.0f)
			_lightRotationAmount = 2.0f;
	}

	if (GetAsyncKeyState(VK_OEM_COMMA))
	{
		_lightRotationAmount -= 0.5f * deltaTime;

		if (_lightRotationAmount < -2.0f)
			_lightRotationAmount = -2.0f;
	}

	if (GetAsyncKeyState('1'))
	{
		// Switch to Spring
		_currentSeason = 0;
		_currentDay = 1;

	}
	if (GetAsyncKeyState('2'))
	{
		// Switch to Summer
		_currentSeason = 1;
		_currentDay = 5;
	}
	if (GetAsyncKeyState('3'))
	{
		// Switch to Autumn
		_currentSeason = 2;
		_currentDay = 9;
	}
	if (GetAsyncKeyState('4'))
	{
		// Switch to Winter
		_currentSeason = 3;
		_currentDay = 13;
	}

	if (GetAsyncKeyState('T'))
	{
		_lightRotationAmount = 0.0f;
	}

	_sceneLight.lightDirection = lightDir;
	_shadows->UpdateLightDirection(_sceneLight.lightDirection);
	_shadows->SetSceneCentre(m_Player->GetPlayerPosition());
	_shadows->BuildShadowTransform();

	for (SceneElement* element : _sceneElements)
	{
		if (element->GetElementName() == "Light Source Sphere")
		{
			element->GetTransform()->SetPosition(_shadows->GetLightPosition());
		}
		element->Update(deltaTime);
	}

	_skyGradient->SetSceneCentre(m_Player->GetPlayerPosition());
	_skyGradient->Update(deltaTime);

	_basicLight->CalculateLightColour(_sceneLight, _shadows->GetLightPosition().y, _sceneFog);

	_heatHaze->Update(Timer::GetDeltaMilliseconds() / 2);

	_diamondSquareTerrain->Update(deltaTime);

	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('R'))
	{
		if (_currentCooldown <= 0.0f)
		{
			_basicLight->SetWireframeMode(!_basicLight->GetWireframeState());
			_currentCooldown = _buttonCooldown;
		}
	}

	if(_currentCooldown > 0.0f)
		_currentCooldown -= deltaTime;

	std::wostringstream out;
	out.precision(6);
	out << L"Day: " << _currentDay << L" - Season: " << _seasonNames.find(_currentSeason)->second.c_str() << L" Time speed: " << _lightRotationAmount;
	//_systemHandlers.GetWindowClass()->SetWindowCaption(out.str().c_str());
}

void MainScene::Draw()
{	
	XMFLOAT3 sunPos = _shadows->GetLightPosition();
	_sceneFog.sunHeight = sunPos.y;

	_basicLight->SetAsCurrentRenderTarget();
	_basicLight->SetAsCurrentViewport();
	RenderClass::DisableRtvClearing();
	_skyGradient->Render(m_Player->GetCamera(), sunPos);

	RenderClass::EnableZBuffer();
	_shadows->Render(_sceneElements);

	_basicLight->Render(m_Player->GetCamera(), _sceneLight, _pointLights, _spotLight, _sceneFog, _sceneElements, *_shadows);
	
	_renderToQuad->SetAsCurrentVertexShader();

	if (_seasonNames.find(_currentSeason)->second == "Summer" || _seasonNames.find(_currentSeason)->second == "Winter")
	{
		_heatHaze->Render(_basicLight->GetRenderTargetSRV(), _seasonNames.find(_currentSeason)->second);
		_renderToQuad->Render(_heatHaze->GetTexture());
	}
	else
	{
		_renderToQuad->Render(_basicLight->GetRenderTargetSRV());
	}
}
