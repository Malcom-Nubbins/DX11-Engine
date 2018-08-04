#include "MainScene.h"

MainScene::MainScene(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass, WindowClass* windowClass, Timer* timer)
	: _d3dClass(d3dClass), _shaderClass(shaderClass), _renderClass(renderClass), _bufferClass(bufferClass), _windowClass(windowClass), _timer(timer)
{
	_camera = nullptr;
	_basicLight = nullptr;
	_shadows = nullptr;
	_skyGradient = nullptr;
	_renderToQuad = nullptr;
	_heatHaze = nullptr;
	_planeVertexBuffer = nullptr;
	_planeIndexBuffer = nullptr;

	_lightRotationAmount = XMConvertToRadians(10.0f);

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
		{ 0, "Spring" },
		{ 1, "Summer" },
		{ 2, "Autumn" },
		{ 3, "Winter" }
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

	delete _camera;
	_camera = nullptr;

	_planeVertexBuffer->Release();
	_planeIndexBuffer->Release();
	_matrixBuffer->Release();
	_objectValueBuffer->Release();

	_sceneElements.clear();
}

void MainScene::ResizeViews(float windowWidth, float windowHeight)
{
	_camera->SetLens(XM_PIDIV4, (windowWidth / windowHeight), 0.01f, 1000.0f);
	//_d3dClass->GetSwapChain()->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	_basicLight->Resize(windowWidth, windowHeight);
	_renderToQuad->Resize(windowWidth, windowHeight);
	_heatHaze->Resize(windowWidth, windowHeight);
}

void MainScene::InitialiseScene(float windowWidth, float windowHeight)
{
	InitialiseSceneGraphics(windowWidth, windowHeight);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_matrixBuffer);

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ObjectValuesBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	_d3dClass->GetDevice()->CreateBuffer(&bd, nullptr, &_objectValueBuffer);

	_camera = new Camera();
	_camera->SetLens(XM_PIDIV4, (windowWidth / windowHeight), 0.01f, 1000.0f);
	_camera->LookAt(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	_sceneLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_sceneLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_sceneLight.LightDirection = XMFLOAT3(5.0f, 0.0f, 0.0f);

	_spotLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	_spotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_spotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_spotLight.Attenuation = XMFLOAT3(1.0f, 0.0f, 0.0f);
	_spotLight.Spot = 20.0f;
	_spotLight.Range = 50.0f;

	_preOffsetLightDir = _sceneLight.LightDirection;

	_sceneFog.FogStart = 5.0f;
	_sceneFog.FogRange = 50.0f;
	_sceneFog.FogColourDay = XMFLOAT4(0.7f, 0.80f, 0.92f, 1.0f);
	_sceneFog.FogColourNight = XMFLOAT4(0.01f, 0.02f, 0.04f, 1.0f);
	_sceneFog.FogColourSunriseSunset = XMFLOAT4(0.89f, 0.59f, 0.27f, 1.0f); 
	
	ObjectMaterial aircraftMat;
	aircraftMat.ambient = XMFLOAT4(0.913, 0.921, 0.925, 1.0f);
	aircraftMat.diffuse = XMFLOAT4(0.913, 0.921, 0.925, 1.0f);
	aircraftMat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);

	ObjectMaterial shiny;
	shiny.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.01f);

	ObjectMaterial charcoal;
	charcoal.ambient = XMFLOAT4(0.02, 0.02, 0.02, 1.0f);
	charcoal.diffuse = XMFLOAT4(0.02, 0.02, 0.02, 1.0f);
	charcoal.specular = XMFLOAT4(0.02, 0.02, 0.02, 0.3f);

	ObjectMaterial matte;
	matte.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	matte.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	matte.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.3f);

	_bufferClass->CreateGroundPlane(&_planeVertexBuffer, &_planeIndexBuffer);

	ObjectMesh planeMesh;
	planeMesh.vertexBuffer = _planeVertexBuffer;
	planeMesh.indexBuffer = _planeIndexBuffer;
	planeMesh.numberOfIndices = 6;
	planeMesh.vertexBufferOffset = 0;
	planeMesh.vertexBufferStride = sizeof(SimpleVertex);

	ObjectMesh aircraftMesh;
	aircraftMesh = OBJLoader::Load((char*)"Core/Resources/Objects/Hercules.obj", _d3dClass->GetDevice(), false);

	_diamondSquareTerrain = new DiamondSquareTerrain(_d3dClass);
	_diamondSquareTerrain->SetTerrainValues(256, 256, 512);
	_diamondSquareTerrain->GenerateTerrain();

	_camera->SetPosition(XMFLOAT3(0.0f, _diamondSquareTerrain->GetHeight(0.0f, -2.0f) + 2.0f, -2.0f));

	ObjectMesh diamondSquareMesh;
	diamondSquareMesh.vertexBuffer = _diamondSquareTerrain->GetVertexBuffer();
	diamondSquareMesh.indexBuffer = _diamondSquareTerrain->GetIndexBuffer();
	diamondSquareMesh.numberOfIndices = static_cast<int>(_diamondSquareTerrain->GetMeshData()->Indices.size());
	diamondSquareMesh.vertexBufferOffset = 0;
	diamondSquareMesh.vertexBufferStride = sizeof(SimpleVertex);

	SceneElement* element = new SceneElement("Ground Plane", diamondSquareMesh, matte);
	element->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	element->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	element->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	element->SetColourTexture(_textureHandler->GetGroundColourTexture());
	element->SetNormalMap(_textureHandler->GetGroundNormalMap());
	element->SetDisplacementMap(_textureHandler->GetGroundDisplacementMap());
	element->SetCastShadows(true);
	element->SetAffectedByLight(true);
	_diamondSquareTerrain->SetGameObject(element);
	_diamondSquareTerrain->SetTerrainFinishedBuilding(true);
	_sceneElements.push_back(element);

	element = new SceneElement("Underworld", planeMesh, shiny);
	element->SetPosition(XMFLOAT3(0.0f, -15.0f, 0.0f));
	element->SetScale(XMFLOAT3(256.0f, 1.0f, 256.0f));
	element->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	element->SetColourTexture(_textureHandler->GetGroundColourTexture());
	element->SetNormalMap(_textureHandler->GetGroundNormalMap());
	element->SetDisplacementMap(_textureHandler->GetGroundDisplacementMap());
	element->SetCastShadows(true);
	element->SetAffectedByLight(true);
	_sceneElements.push_back(element);

	ObjectMesh cube = OBJLoader::Load((char*)"Core/Resources/Objects/spherex.obj", _d3dClass->GetDevice(), false);
	srand(time(NULL));
	for (int i = 0; i < 90; ++i)
	{
		element = new SceneElement("Cube", cube, charcoal);
		float randomX = MathsHandler::RandomFloat(-128, 128);
		float randomZ = MathsHandler::RandomFloat(-128, 128);

		element->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ) + 2.0f, randomZ));
		element->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		element->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		//element->SetColourTexture(_textureHandler->GetStoneTexture());
		//element->SetNormalMap(_textureHandler->GetStoneNormalMap());
		//element->SetDisplacementMap(_textureHandler->GetStoneDisplacementMap());
		element->SetCastShadows(true);
		element->SetAffectedByLight(true);

		_sceneElements.push_back(element);
	}

	element = new SceneElement("Aircraft", aircraftMesh, aircraftMat);
	element->SetPosition(XMFLOAT3(0.0f, _diamondSquareTerrain->GetHeight(0, 0) + 8.0f, 0));
	element->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	element->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	element->SetColourTexture(_textureHandler->GetAircraftTexture());
	element->SetNormalMap(_textureHandler->GetAircraftNormalMap());
	//element->SetDisplacementMap(_textureHandler->GetStoneDisplacementMap());
	element->SetCastShadows(true);
	element->SetAffectedByLight(true);

	_sceneElements.push_back(element);

	ObjectMesh plant0 = OBJLoader::Load((char*)"Core/Resources/Objects/plant0.obj", _d3dClass->GetDevice(), true);
	ObjectMesh plant1 = OBJLoader::Load((char*)"Core/Resources/Objects/plant1.obj", _d3dClass->GetDevice(), true);
	
	PointLight light;
	light.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	light.Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	light.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	light.Attenuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
	light.Range = 25.0f;
	light.Position = _camera->GetPosition();
	//_pointLights.push_back(light);

	/*for (int i = 0; i < 60; ++i)
	{
		float randomX = MathsHandler::RandomFloat(-90, 90);
		float randomZ = MathsHandler::RandomFloat(-90, 90);
		
		element = new SceneElement("Plant " + i, plant0, shiny);
		element->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ), randomZ));
		element->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		element->SetRotation(XMFLOAT3(0.0f, XMConvertToRadians(rand()), 0.0f));
		element->SetColourTexture(nullptr);
		element->SetCastShadows(true);
		element->SetAffectedByLight(true);

		light.Position = element->GetPosition();
		_pointLights.push_back(light);

		_sceneElements.push_back(element);

		randomX = MathsHandler::RandomFloat(-90, 90);
		randomZ = MathsHandler::RandomFloat(-90, 90);

		element = new SceneElement("Plant " + i, plant1, shiny);
		element->SetPosition(XMFLOAT3(randomX, _diamondSquareTerrain->GetHeight(randomX, randomZ), randomZ));
		element->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		element->SetRotation(XMFLOAT3(0.0f, XMConvertToRadians(rand()), 0.0f));
		element->SetColourTexture(nullptr);
		element->SetCastShadows(true);
		element->SetAffectedByLight(true);

		light.Position = element->GetPosition();
		_pointLights.push_back(light);

		_sceneElements.push_back(element);
	}*/

	element = new SceneElement("Light Source Sphere", OBJLoader::Load((char*)"Core/Resources/Objects/sphere.obj", _d3dClass->GetDevice(), false), shiny);
	element->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	element->SetScale(XMFLOAT3(7.0f, 7.0f, 7.0f));
	element->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
	element->SetColourTexture(nullptr);
	element->SetCastShadows(false);
	element->SetAffectedByLight(false);
	_sceneElements.push_back(element);
}

void MainScene::InitialiseSceneGraphics(float windowWidth, float windowHeight)
{
	_textureHandler = new TextureHandler(_d3dClass);
	_textureHandler->LoadAllTextures();

	_basicLight = new BasicLight(_d3dClass, _shaderClass, _renderClass, _bufferClass);
	_basicLight->Initialise(windowWidth, windowHeight);

	_shadows = new Shadows(_d3dClass, _renderClass, _shaderClass, _bufferClass);
	_shadows->Initialise(8192, 8192);

	_skyGradient = new SkyColourGradient(_d3dClass, _renderClass, _shaderClass, _bufferClass);
	_skyGradient->Initialise();

	_renderToQuad = new RenderToFullscreenQuad(_d3dClass, _shaderClass, _renderClass, _bufferClass);
	_renderToQuad->Initialise(windowWidth, windowHeight);

	_heatHaze = new HeatHaze(_d3dClass, _shaderClass, _renderClass, _bufferClass);
	_heatHaze->Initialise(windowWidth, windowHeight);
}

void MainScene::HandleMouse(WPARAM btnState, int x, int y)
{
	float dx = XMConvertToRadians(0.25f *static_cast<float>(x - _lastMousePos.x));
	float dy = XMConvertToRadians(0.25f * static_cast<float>(y - _lastMousePos.y));

	if ((btnState & MK_LBUTTON) != 0)
	{
		_camera->Pitch(dy);
		_camera->Yaw(dx);
	}

	_lastMousePos = XMFLOAT2(x, y);
}

void MainScene::Update(float deltaTime)
{	
	_camera->Update(deltaTime);

	_spotLight.Position = _camera->GetPosition();
	_spotLight.Direction = _camera->GetLookDirection();
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

	_sceneLight.LightDirection = lightDir;
	_shadows->UpdateLightDirection(_sceneLight.LightDirection);
	_shadows->SetSceneCentre(_camera->GetPosition());
	_shadows->BuildShadowTransform();

	for (SceneElement* element : _sceneElements)
	{
		if (element->GetElementName() == "Light Source Sphere")
		{
			element->SetPosition(_shadows->GetLightPosition());
		}
		element->Update(deltaTime);
	}

	_skyGradient->SetSceneCentre(_camera->GetPosition());
	_skyGradient->Update(deltaTime);

	_basicLight->CalculateLightColour(_sceneLight, _shadows->GetLightPosition().y, _sceneFog);

	_heatHaze->Update(_timer->GameTime() / 2);

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
	_windowClass->SetWindowCaption(out.str().c_str());
}

void MainScene::Draw()
{	
	XMFLOAT3 sunPos = _shadows->GetLightPosition();
	_sceneFog.sunHeight = sunPos.y;

	_basicLight->SetAsCurrentRenderTarget();
	_basicLight->SetAsCurrentViewport();
	_skyGradient->Render(_matrixBuffer, *_camera, sunPos);

	_renderClass->EnableZBuffer();
	_shadows->Render(_sceneElements, *_diamondSquareTerrain);

	_basicLight->Render(*_camera, _sceneLight, _pointLights, _spotLight, _sceneFog, _sceneElements, *_diamondSquareTerrain, _matrixBuffer, _objectValueBuffer, *_shadows);
	
	_renderToQuad->SetAsCurrentVertexShader();

	if (_seasonNames.find(_currentSeason)->second == "Summer" || _seasonNames.find(_currentSeason)->second == "Winter")
	{
		_heatHaze->Render(_basicLight->GetRenderTargetSRV(), _textureHandler, _seasonNames.find(_currentSeason)->second);
		_renderToQuad->Render(_heatHaze->GetTexture());
	}
	else
	{
		_renderToQuad->Render(_basicLight->GetRenderTargetSRV());
	}
}
