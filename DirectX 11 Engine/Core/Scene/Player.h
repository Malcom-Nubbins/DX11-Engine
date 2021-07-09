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

	const float m_LookSpeed = 0.25f;
	const float m_MovementSpeed = 3.0f;
private:
	

public:
	void SetPlayerPosition(XMFLOAT3 pos) const;
	void UpdatePlayerLookDirection(MouseMotionEvent& e);
	void UpdatePlayerPosition(KeyEvent& e);

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

	void Update(float delta);
};
