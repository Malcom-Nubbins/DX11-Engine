#include "TestingScene.h"

TestingScene::TestingScene(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass,
	BufferClass* bufferClass, WindowClass* windowClass, TextureHandler* textureHandler, Timer* timer)
	: Scene(d3dClass, shaderClass, renderClass, bufferClass, windowClass, textureHandler, timer)
{
}

TestingScene::~TestingScene()
{
	
}

void TestingScene::InitialiseScene(float windowWidth, float windowHeight)
{
	Scene::InitialiseScene(windowWidth, windowHeight);

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

	ObjectMaterial aircraftMat;
	aircraftMat.ambient = XMFLOAT4(1.000, 0.766, 0.336, 1.0f);
	aircraftMat.diffuse = XMFLOAT4(1.000, 0.766, 0.336, 1.0f);
	aircraftMat.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);

	ObjectMaterial shiny;
	shiny.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shiny.specular = XMFLOAT4(0.560f, 0.570f, 0.580f, 0.8f);

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

	auto* groundTransform = new Transform();
	groundTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	groundTransform->SetScale(XMFLOAT3(10.0f, 1.0f, 10.0f));
	groundTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto* groundAppearance = new Appearance(planeMesh, shiny);
	groundAppearance->SetColourTexture(_textureHandler->GetMetalFloorColourTexture());
	groundAppearance->SetNormalMap(_textureHandler->GetMetalFloorNormalMap());
	//groundAppearance->SetDisplacementMap(_textureHandler->GetGroundDisplacementMap());
	//groundAppearance->SetSpecularMap(_textureHandler->GetGroundSpecularMap());

	//Scene elements
	{
		auto* groundPlaneElement = new SceneElement("Ground Plane", groundTransform, groundAppearance);
		groundPlaneElement->SetCastShadows(true);
		groundPlaneElement->SetAffectedByLight(true);
		_sceneElements.push_back(groundPlaneElement);
	}
}

void TestingScene::InitialiseSceneGraphics(float windowWidth, float windowHeight)
{
	_basicLight = new BasicLight(_d3dClass, _shaderClass, _renderClass, _bufferClass);
	_basicLight->Initialise(windowWidth, windowHeight);

	_shadows = new Shadows(_d3dClass, _renderClass, _shaderClass, _bufferClass);
	_shadows->Initialise(8192, 8192);

	_skyGradient = new SkyColourGradient(_d3dClass, _renderClass, _shaderClass, _bufferClass);
	_skyGradient->Initialise();

	_renderToQuad = new RenderToFullscreenQuad(_d3dClass, _shaderClass, _renderClass, _bufferClass);
	_renderToQuad->Initialise(windowWidth, windowHeight);
}

void TestingScene::Cleanup()
{
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
	_matrixBuffer->Release();
	_objectValueBuffer->Release();

	_sceneElements.clear();
}

void TestingScene::ResizeViews(float windowWidth, float windowHeight)
{
	Scene::ResizeViews(windowWidth, windowHeight);
	_basicLight->Resize(windowWidth, windowHeight);
	_renderToQuad->Resize(windowWidth, windowHeight);
}

void TestingScene::HandleMouse(WPARAM btnState, int x, int y)
{
	Scene::HandleMouse(btnState, x, y);
}

void TestingScene::Update(float delta)
{
	Scene::Update(delta);

	_spotLight.position = _camera->GetPosition();
	_spotLight.direction = _camera->GetLookDirection();

	_shadows->UpdateLightDirection(_sceneLight.lightDirection);
	_shadows->SetSceneCentre(_camera->GetPosition());
	_shadows->BuildShadowTransform();

	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('R'))
	{
		if (_currentCooldown <= 0.0f)
		{
			_basicLight->SetWireframeMode(!_basicLight->GetWireframeState());
			_currentCooldown = _buttonCooldown;
		}
	}

	_skyGradient->SetSceneCentre(_camera->GetPosition());
	_skyGradient->Update(delta);

	for (auto element : _sceneElements)
	{
		element->Update(delta);
	}

	if (_currentCooldown > 0.0f)
		_currentCooldown -= delta;

	std::wostringstream out;
	out << L"Running Test Scene";
	_windowClass->SetWindowCaption(out.str().c_str());
}

void TestingScene::Draw()
{
	_basicLight->SetAsCurrentRenderTarget();
	_basicLight->SetAsCurrentViewport();
	_skyGradient->Render(_matrixBuffer, *_camera, XMFLOAT3(0.0f, 50.0f, 0.0f));

	_renderClass->EnableZBuffer();
	_shadows->Render(_sceneElements);

	_basicLight->Render(*_camera, _sceneLight, _spotLight, _sceneElements, _matrixBuffer, _objectValueBuffer, *_shadows);

	_renderToQuad->SetAsCurrentVertexShader();
	_renderToQuad->Render(_basicLight->GetRenderTargetSRV());
}
