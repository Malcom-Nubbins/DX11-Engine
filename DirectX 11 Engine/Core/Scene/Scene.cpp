#include "Scene.h"

Scene::Scene(Player& player) : m_Player(&player)
{
}

Scene::~Scene()
{
	Scene::Cleanup();
}

void Scene::Cleanup()
{

}

void Scene::PreResizeViews()
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
