#pragma once
#include "../Globals/Structs.h"
#include "../Timer.h"
#include "Player.h"
#include "../Handlers/SystemHandlers.h"

class Scene
{
protected:
	const SystemHandlers& _systemHandlers;
	const Player& _player;

	const Timer& _timer;
	DirectionalLight _sceneLight;
	XMFLOAT2 _lastMousePos;

public:
	Scene(const SystemHandlers& systemHandlers, const Timer& timer, const Player& player);
	virtual ~Scene();
	virtual void Cleanup() = 0;

	virtual void ResizeViews(float windowWidth, float windowHeight) = 0;

	virtual void InitialiseScene(float windowWidth, float windowHeight) = 0;
	virtual void InitialiseSceneGraphics(float windowWidth, float windowHeight) = 0;

	virtual void Update(float delta) = 0;
	virtual void Draw() = 0;
};
