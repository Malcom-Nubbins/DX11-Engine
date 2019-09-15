#include "Player.h"

Player::Player(WindowClass* windowClass) : _camera(nullptr), _windowClass(windowClass)
{
}

Player::~Player()
{
}

void Player::Cleanup()
{
	delete _camera;
	_camera = nullptr;
}

void Player::ResetPlayerCamera(float windowWidth, float windowHeight) const
{
	if(_camera != nullptr)
		_camera->SetLens(XM_PIDIV4, windowWidth, windowHeight, 0.01f, 1000.0f);
}

void Player::Initialise()
{
	_camera = new Camera(_windowClass);
	_camera->SetLens(XM_PIDIV4, _windowClass->GetWindowWidth(), _windowClass->GetWindowHeight(), 0.01f, 1000.0f);
	_camera->LookAt(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	_camera->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));
}

void Player::UpdatePlayerLookDirection(float delta)
{
	POINT state = InputHandler::GetMousePos(_windowClass->GetHWND());

	auto yaw = (state.x - _windowClass->GetScreenCentre().x) * _lookSpeed;
	auto pitch = (state.y - _windowClass->GetScreenCentre().y) * _lookSpeed;

	_camera->Pitch(pitch * delta);
	_camera->Yaw(yaw * delta);

	_camera->Update(delta);

	InputHandler::SetMousePos(_windowClass->GetHWND(), _windowClass->GetScreenCentre());
}

void Player::UpdatePlayerPosition(float delta)
{
	if (InputHandler::IsKeyDown(A))
	{
		_camera->Strafe(-_movementSpeed * delta);
	}

	if (InputHandler::IsKeyDown(D))
	{
		_camera->Strafe(_movementSpeed * delta);
	}
	if (InputHandler::IsKeyDown(W))
	{
		_camera->Walk(_movementSpeed * delta);
	}

	if (InputHandler::IsKeyDown(S))
	{
		_camera->Walk(-_movementSpeed * delta);
	}
}

void Player::SetPlayerPosition(XMFLOAT3 pos) const
{
	_camera->SetPosition(pos);
}

void Player::Update(float delta)
{
	if(!InputHandler::IsKeyDown(LeftAlt))
	{
		UpdatePlayerLookDirection(delta);
		UpdatePlayerPosition(delta);
	}
}
