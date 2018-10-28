#include "Scene.h"
#include "../Handlers/InputHandler.h"

Scene::Scene(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass,
	WindowClass* windowClass, TextureHandler* textureHandler, Timer* timer)
	: _d3dClass(d3dClass), _shaderClass(shaderClass), _renderClass(renderClass),
	  _bufferClass(bufferClass), _windowClass(windowClass), _textureHandler(textureHandler), _timer(timer), 
	  _camera(nullptr), _sceneLight()
{
}

Scene::~Scene()
{
	Scene::Cleanup();
}

void Scene::Cleanup()
{
	delete _camera;
	_camera = nullptr;
}

void Scene::ResizeViews(float windowWidth, float windowHeight)
{
	_camera->SetLens(XM_PIDIV4, windowWidth, windowHeight, 0.01f, 1000.0f);
}

void Scene::InitialiseScene(float windowWidth, float windowHeight)
{
	_camera = new Camera(_windowClass);
	_camera->SetLens(XM_PIDIV4, windowWidth, windowHeight, 0.01f, 1000.0f);
	_camera->LookAt(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	_camera->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void Scene::InitialiseSceneGraphics(float windowWidth, float windowHeight)
{
}

void Scene::HandleMouse()
{
	const float lookSpeed = 0.0025f;
	POINT state = InputHandler::GetMousePos(_windowClass->GetHWND());
	POINT screenCentre;
	screenCentre.x = _windowClass->GetWindowWidth() / 2;
	screenCentre.y = _windowClass->GetWindowHeight() / 2;

	auto yaw = (state.x - screenCentre.x) * lookSpeed;
	auto pitch = (state.y - screenCentre.y) * lookSpeed;

	_camera->Pitch(pitch);
	_camera->Yaw(yaw);

	InputHandler::SetMousePos(_windowClass->GetHWND(), screenCentre);
}

void Scene::Update(float delta)
{
	_camera->Update(delta);
}

void Scene::Draw()
{
}
