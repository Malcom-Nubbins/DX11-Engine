#include "Player.h"
#include "../DX11Engine.h"
#include "../ApplicationNew.h"

Player::Player() 
	: m_Camera(nullptr)
	, _windowClass(nullptr)
	, m_CurrMovementState(MovementState::None)
	, m_Sprint(false)
	, m_LockMouse(true)
	, m_CurrCamPitch(0.f)
	, m_CurrCamYaw(0.f)
{
}

Player::~Player()
{
}

void Player::Cleanup()
{
	delete m_Camera;
	m_Camera = nullptr;
}

void Player::ResetPlayerCamera(float windowWidth, float windowHeight) const
{
	if (m_Camera != nullptr)
		m_Camera->Reset((85.0f / 180.0f) * XM_PI, 0.01f, 1000.0f, (windowWidth / windowHeight));
}

void Player::Initialise()
{
	DX11Engine const& engine = DX11Engine::Get();

	float aspect = static_cast<float>(engine.GetClientWidth()) / static_cast<float>(engine.GetClientHeight());

	m_Camera = new Camera(
		XMFLOAT3(0.0f, 1.0f, 0.0f), 
		XMFLOAT3(1.0f, 0.0f, 0.0f), 
		XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		0.01f, 1000.0f,
		aspect, (60.0f / 180.0f) * XM_PI);
}

void Player::OnMouseButtonDown(MouseButtonEvent& e)
{
	if (e.RightMouseButton)
	{
		m_Camera->SetFOV((45.0f / 180.0f) * XM_PI);
	}

	if (e.MiddleMouseButton)
	{
		ShowCursor(false);
		m_LockMouse = true;
	}
}

void Player::OnMouseButtonUp(MouseButtonEvent& e)
{
	if (!e.RightMouseButton)
	{
		m_Camera->SetFOV((60.0f / 180.0f) * XM_PI);
	}
}

void Player::UpdatePlayerLookDirection(MouseMotionEvent& e)
{
	if (m_LockMouse)
	{
		auto window = ApplicationNew::Get().GetWindowByName(L"DX11 Engine");
		POINT screenCentre = window->GetScreenCentre();

		m_CurrCamYaw = (e.X - screenCentre.x) * m_LookSpeed;
		m_CurrCamPitch = (e.Y - screenCentre.y) * m_LookSpeed;

		InputHandler::SetMousePos(window->GetHWND(), window->GetScreenCentre());
	}
}

void Player::OnPlayerMovementKeyPressed(KeyEvent& e)
{
	using namespace KeyCode;
	if (e.Key == Key::A)
	{
		m_CurrMovementState = m_CurrMovementState | MovementState::Left;
	}

	if (e.Key == Key::D)
	{
		m_CurrMovementState = m_CurrMovementState | MovementState::Right;
	}

	if (e.Key == Key::W)
	{
		m_CurrMovementState = m_CurrMovementState | MovementState::Forward;
	}

	if (e.Key == Key::S)
	{
		m_CurrMovementState = m_CurrMovementState | MovementState::Backward;
	}

	m_Sprint = e.Shift;
}

void Player::OnPlayerMovementKeyReleased(KeyEvent& e)
{
	using namespace KeyCode;
	if (e.Key == Key::A)
	{
		if ((MovementState::Left & m_CurrMovementState) == MovementState::Left)
		{
			m_CurrMovementState = m_CurrMovementState & ~MovementState::Left;
		}
	}

	if (e.Key == Key::D)
	{
		if ((MovementState::Right & m_CurrMovementState) == MovementState::Right)
		{
			m_CurrMovementState = m_CurrMovementState & ~MovementState::Right;
		}
	}

	if (e.Key == Key::W)
	{
		if ((MovementState::Forward & m_CurrMovementState) == MovementState::Forward)
		{
			m_CurrMovementState = m_CurrMovementState & ~MovementState::Forward;
		}
	}

	if (e.Key == Key::S)
	{
		if ((MovementState::Backward & m_CurrMovementState) == MovementState::Backward)
		{
			m_CurrMovementState = m_CurrMovementState & ~MovementState::Backward;
		}
	}

	m_Sprint = e.Shift;
}

void Player::SetPlayerPosition(XMFLOAT3 pos) const
{
	m_Camera->SetPosition(pos);
}


void Player::Update(double delta)
{
	bool shouldMoveLeft = ((MovementState::Left & m_CurrMovementState) == MovementState::Left);
	bool shouldMoveRight = ((MovementState::Right & m_CurrMovementState) == MovementState::Right);
	bool shouldMoveForward = ((MovementState::Forward & m_CurrMovementState) == MovementState::Forward);
	bool shouldMoveBackward = ((MovementState::Backward & m_CurrMovementState) == MovementState::Backward);

	double moveSpeed = m_Sprint ? (m_MovementSpeed * 2.5f) : m_MovementSpeed;
	moveSpeed *= delta;

	if (InputHandler::IsKeyDown(Keys::M))
	{
		ShowCursor(true);
		m_LockMouse = false;
	}

	if (shouldMoveLeft)
	{
		m_Camera->Strafe(-moveSpeed);
	}

	if (shouldMoveRight)
	{
		m_Camera->Strafe(moveSpeed);
	}

	if (shouldMoveForward)
	{
		m_Camera->Walk(moveSpeed);
	}

	if (shouldMoveBackward)
	{
		m_Camera->Walk(-moveSpeed);
	}

	m_Camera->Pitch(m_CurrCamPitch * static_cast<float>(delta));
	m_Camera->Yaw(m_CurrCamYaw * static_cast<float>(delta));

	m_Camera->Update(static_cast<float>(delta));
}
