#include "Camera.h"
#include "../DX11Engine.h"

Camera::Camera(XMFLOAT3 eye, XMFLOAT3 right, XMFLOAT3 up, XMFLOAT3 at, float nearZ, float farZ, float aspect, float fov, bool orthographic)
	: m_Eye(eye)
	, m_Right(right)
	, m_Up(up)
	, m_At(at)
	, m_Near(nearZ)
	, m_Far(farZ)
	, m_Aspect(aspect)
	, m_FovY(fov)
	, m_OrthographicMode(orthographic)
{
	SetLens();
	LookAt();
}

Camera::~Camera()
{
}

void Camera::SetPerspective(const bool usePerspective)
{
	m_OrthographicMode = !usePerspective;
}

void Camera::SetLens()
{
	DX11Engine const& engine = DX11Engine::Get();

	auto proj = XMMatrixOrthographicLH(engine.GetClientWidth(), engine.GetClientHeight(), m_Near, m_Far);
	XMStoreFloat4x4(&m_OthographicProj, proj);

	const auto camRight = XMLoadFloat3(&m_Right);
	const auto camAt = XMLoadFloat3(&m_At);
	const auto camUp = XMLoadFloat3(&m_Up);

	proj = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far);
	XMStoreFloat4x4(&m_PerspectiveProj, proj);
	//XMStoreFloat4x4(&_defaultView, XMMatrixLookAtLH(camRight, camAt, camUp));
}

void Camera::LookAt()
{
	const auto camRight = XMLoadFloat3(&m_Right);
	const auto camAt = XMLoadFloat3(&m_At);
	const auto camUp = XMLoadFloat3(&m_Up);

	XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(camRight, camAt, camUp));
	XMStoreFloat4x4(&m_DefaultView, XMMatrixLookAtLH(camRight, camAt, camUp));
}

void Camera::SetPosition(const XMFLOAT3 position)
{
	m_Eye = position;
}

void Camera::SetFOV(float fov)
{
	m_FovY = fov;

	auto proj = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far);
	XMStoreFloat4x4(&m_PerspectiveProj, proj);
}

void Camera::Reset(float fov, float nearZ, float farZ, float aspect)
{
	m_FovY = fov;
	m_Near = nearZ;
	m_Far = farZ;
	m_Aspect = aspect;

	SetLens();
}

void Camera::Strafe(const float amount)
{
	const auto strafe = XMVectorReplicate(amount);
	const auto right = XMLoadFloat3(&m_Right);
	const auto eye = XMLoadFloat3(&m_Eye);
	XMStoreFloat3(&m_Eye, XMVectorMultiplyAdd(strafe, right, eye));
}

void Camera::Walk(float amount)
{
	const auto walk = XMVectorReplicate(amount);
	const auto at = XMLoadFloat3(&m_At);
	const auto eye = XMLoadFloat3(&m_Eye);
	XMStoreFloat3(&m_Eye, XMVectorMultiplyAdd(walk, at, eye));
}

void Camera::Pitch(const float angle)
{
	if(m_Up.y < 0)
	{
		if(m_At.y < 0 && angle > 0)
			return;
	}
	else if(m_Up.y > 0)
	{
		if(m_At.y > 0.98f && angle < 0)
			return;
	}

	const auto pitch = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), angle);

	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), pitch));
	XMStoreFloat3(&m_At, XMVector3TransformNormal(XMLoadFloat3(&m_At), pitch));
}

void Camera::Yaw(const float angle)
{
	const auto yaw = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), yaw));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), yaw));
	XMStoreFloat3(&m_At, XMVector3TransformNormal(XMLoadFloat3(&m_At), yaw));
}

void Camera::Update(const float deltaTime)
{
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	auto r = XMLoadFloat3(&m_Right);
	auto u = XMLoadFloat3(&m_Up);
	auto l = XMLoadFloat3(&m_At);
	const auto p = XMLoadFloat3(&m_Eye);

	l = XMVector3Normalize(l); // Normalise the Look vector
	u = XMVector3Normalize(XMVector3Cross(l, r)); // Normalise the Up vector
	r = XMVector3Cross(u, l); // Normalise the Right vector

	const auto x = -XMVectorGetX(XMVector3Dot(p, r));
	const auto y = -XMVectorGetX(XMVector3Dot(p, u));
	const auto z = -XMVectorGetX(XMVector3Dot(p, l));

	XMStoreFloat3(&m_Right, r);
	XMStoreFloat3(&m_Up, u);
	XMStoreFloat3(&m_At, l);

	(m_View)(0, 0) = m_Right.x;
	(m_View)(1, 0) = m_Right.y;
	(m_View)(2, 0) = m_Right.z;
	(m_View)(3, 0) = x;

	(m_View)(0, 1) = m_Up.x;
	(m_View)(1, 1) = m_Up.y;
	(m_View)(2, 1) = m_Up.z;
	(m_View)(3, 1) = y;

	(m_View)(0, 2) = m_At.x;
	(m_View)(1, 2) = m_At.y;
	(m_View)(2, 2) = m_At.z;
	(m_View)(3, 2) = z;

	(m_View)(0, 3) = 0.0f;
	(m_View)(1, 3) = 0.0f;
	(m_View)(2, 3) = 0.0f;
	(m_View)(3, 3) = 1.0f;
}
