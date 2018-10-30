#pragma once
#include "../Globals/Structs.h"
#include "../Timer.h"
#include "Player.h"
#include "../Handlers/SystemHandlers.h"

class Scene
{
protected:
	SystemHandlers* _systemHandlers;
	Player* _player;

	Timer* _timer;
	DirectionalLight _sceneLight;
	XMFLOAT2 _lastMousePos;

public:
	Scene(SystemHandlers* systemHandlers, Timer* timer, Player* player);
	virtual ~Scene();
	virtual void Cleanup();

	virtual void ResizeViews(float windowWidth, float windowHeight);

	virtual void InitialiseScene(float windowWidth, float windowHeight);
	virtual void InitialiseSceneGraphics(float windowWidth, float windowHeight);

	virtual void Update(float delta);
	virtual void Draw();
};
