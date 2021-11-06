#pragma once
#include "Camera.h"
#include "../Handlers/System Handlers/WindowClass.h"
#include "../Handlers/System Handlers/InputHandler.h"

#include "../Globals/Events.h"

class Player
{
private:
	WindowClass* _windowClass;

	Camera * m_Camera;

	MovementState m_CurrMovementState;

	bool m_LockMouse;

	const float m_LookSpeed = 1.0f;
	const float m_MovementSpeed = 6.0f;
	bool m_Sprint;

	float m_CurrCamYaw;
	float m_CurrCamPitch;
private:
	

public:
	void SetPlayerPosition(XMFLOAT3 pos) const;
	void UpdatePlayerLookDirection(MouseMotionEvent& e);
	void OnPlayerMovementKeyPressed(KeyEvent& e);
	void OnPlayerMovementKeyReleased(KeyEvent& e);

	XMFLOAT3 GetPlayerPosition() const { return m_Camera->GetPosition(); }
	XMFLOAT3 GetPlayerLookDirection() const { return m_Camera->GetLookDirection(); }

	Camera& GetCamera() const { return *m_Camera; }
public:
	Player();
	~Player();

	void Cleanup();

	void ResetPlayerCamera(float windowWidth, float windowHeight) const;
	void Initialise();

	void OnMouseButtonDown(MouseButtonEvent& e);
	void OnMouseButtonUp(MouseButtonEvent& e);

	void Update(double delta);
};
