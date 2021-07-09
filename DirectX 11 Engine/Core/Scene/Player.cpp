#include "Player.h"
#include "../DX11Engine.h"
#include "../ApplicationNew.h"

Player::Player() : m_Camera(nullptr), _windowClass(nullptr)
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
		aspect, (85.0f / 180.0f) * XM_PI);
}

void Player::OnMouseButtonDown(MouseButtonEvent& e)
{
	if (e.RightMouseButton)
	{
		m_Camera->SetFOV((55.0f / 180.0f) * XM_PI);
	}
}

void Player::OnMouseButtonUp(MouseButtonEvent& e)
{
	if (!e.RightMouseButton)
	{
		m_Camera->SetFOV((85.0f / 180.0f) * XM_PI);
	}
}

void Player::UpdatePlayerLookDirection(MouseMotionEvent& e)
{
	auto window = ApplicationNew::Get().GetWindowByName(L"DX11 Engine");
	POINT screenCentre = window->GetScreenCentre();

	auto yaw = (e.X - screenCentre.x) * m_LookSpeed;
	auto pitch = (e.Y - screenCentre.y) * m_LookSpeed;

	m_Camera->Pitch(pitch * Timer::GetDeltaMilliseconds());
	m_Camera->Yaw(yaw * Timer::GetDeltaMilliseconds());

	m_Camera->Update(Timer::GetDeltaMilliseconds());

	InputHandler::SetMousePos(window->GetHWND(), window->GetScreenCentre());
}

void Player::UpdatePlayerPosition(KeyEvent& e)
{
	switch (e.Key)
	{
	case KeyCode::Key::A:
		{
			m_Camera->Strafe(-m_MovementSpeed * Timer::GetDeltaMilliseconds());
		}
		break;
	case KeyCode::Key::D:
		{
			m_Camera->Strafe(m_MovementSpeed * Timer::GetDeltaMilliseconds());
		}
		break;
	case KeyCode::Key::W:
		{
			m_Camera->Walk(m_MovementSpeed * Timer::GetDeltaMilliseconds());
		}
		break;
	case KeyCode::Key::S:
		{
			m_Camera->Walk(-m_MovementSpeed * Timer::GetDeltaMilliseconds());
		}
		break;
	default:
		break;
	}
}

void Player::SetPlayerPosition(XMFLOAT3 pos) const
{
	m_Camera->SetPosition(pos);
}


void Player::Update(float delta)
{

}
