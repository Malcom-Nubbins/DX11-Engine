#include "Scene.h"
#include "../Handlers/InputHandler.h"

Scene::Scene(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass,
	WindowClass* windowClass, TextureHandler* textureHandler, Timer* timer, Player* player)
	: _d3dClass(d3dClass), _shaderClass(shaderClass), _renderClass(renderClass),
	  _bufferClass(bufferClass), _windowClass(windowClass), _textureHandler(textureHandler), _timer(timer), 
	  _sceneLight(), _player(player)
{
}

Scene::~Scene()
{
	Scene::Cleanup();
}

void Scene::Cleanup()
{

}

void Scene::ResizeViews(float windowWidth, float windowHeight)
{
	
}

void Scene::InitialiseScene(float windowWidth, float windowHeight)
{
	
}

void Scene::InitialiseSceneGraphics(float windowWidth, float windowHeight)
{
}

void Scene::Update(float delta)
{
}

void Scene::Draw()
{
}
