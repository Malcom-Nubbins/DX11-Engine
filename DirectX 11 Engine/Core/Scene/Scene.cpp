#include "Scene.h"

Scene::Scene(SystemHandlers* systemHandler, Timer* timer, Player* player)
	: _systemHandlers(systemHandler), _player(player), _timer(timer), _sceneLight()
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
