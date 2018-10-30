#pragma once
#include "Camera.h"
#include "../Handlers/System Handlers/WindowClass.h"
#include "../Handlers/System Handlers/InputHandler.h"

class Player
{
private:
	WindowClass* _windowClass;

	Camera * _camera;

	const float _lookSpeed = 0.25f;
	const float _movementSpeed = 3.0f;
private:
	void UpdatePlayerLookDirection(float delta);
	void UpdatePlayerPosition(float delta);

public:
	void SetPlayerPosition(XMFLOAT3 pos);

	XMFLOAT3 GetPlayerPosition() { return _camera->GetPosition(); }
	XMFLOAT3 GetPlayerLookDirection() { return _camera->GetLookDirection(); }

	Camera* GetCamera() { return _camera; }
public:
	Player(WindowClass* windowClass);
	~Player();

	void Cleanup();

	void ResetPlayerCamera(float windowWidth, float windowHeight) const;
	void Initialise();

	void Update(float delta);
};
