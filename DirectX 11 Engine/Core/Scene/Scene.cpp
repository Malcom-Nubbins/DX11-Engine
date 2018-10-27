#include "Scene.h"

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
}

void Scene::InitialiseSceneGraphics(float windowWidth, float windowHeight)
{
}

void Scene::HandleMouse(WPARAM btnState, int x, int y)
{
}

void Scene::Update(float delta)
{
	_camera->Update(delta);
}

void Scene::Draw()
{
}
