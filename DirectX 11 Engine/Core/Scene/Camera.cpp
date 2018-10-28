#include "Camera.h"
#include "../Handlers/InputHandler.h"

Camera::Camera(WindowClass* windowClass)
		:_windowClass(windowClass), _nearZ(0), _farZ(0), _aspect(0), _fovY(0), _nearWindowHeight(0), _nearWindowWidth(0),
         _orthographicMode(false)
{
}

Camera::Camera(const Camera& copy): _nearZ(0), _farZ(0), _aspect(0), _fovY(0), _nearWindowHeight(0),
                                    _nearWindowWidth(0), _orthographicMode(false)
{
}


Camera::~Camera()
= default;

void Camera::SetPerspective(const bool usePerspective)
{
	_orthographicMode = !usePerspective;
}

void Camera::SetLens(const float fov, const float width, const float height, const float nearZ, const float farZ)
{
	_fovY = fov;
	_aspect = (width / height);
	_nearZ = nearZ;
	_farZ = farZ;

	auto proj = XMMatrixOrthographicLH(width, height, _nearZ, _farZ);
	XMStoreFloat4x4(&_othographicProj, proj);

	proj = XMMatrixPerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
	XMStoreFloat4x4(&_perspectiveProj, proj);
}

void Camera::LookAt(const XMFLOAT3 right, const XMFLOAT3 at, const XMFLOAT3 up)
{
	_right = right;
	_at = at;
	_up = up;

	const auto camRight = XMLoadFloat3(&_right);
	const auto camAt = XMLoadFloat3(&_at);
	const auto camUp = XMLoadFloat3(&_up);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(camRight, camAt, camUp));
}

void Camera::SetPosition(const XMFLOAT3 position)
{
	_eye = position;
}

void Camera::Strafe(const float amount)
{
	const auto strafe = XMVectorReplicate(amount);
	const auto right = XMLoadFloat3(&_right);
	const auto eye = XMLoadFloat3(&_eye);
	XMStoreFloat3(&_eye, XMVectorMultiplyAdd(strafe, right, eye));
}

void Camera::Walk(float amount)
{
	const auto walk = XMVectorReplicate(amount);
	const auto at = XMLoadFloat3(&_at);
	const auto eye = XMLoadFloat3(&_eye);
	XMStoreFloat3(&_eye, XMVectorMultiplyAdd(walk, at, eye));
}

void Camera::Pitch(const float angle)
{
	if(_up.y < 0)
	{
		if(_at.y < 0 && angle > 0)
			return;
	}
	else if(_up.y > 0)
	{
		if(_at.y > 0.95f && angle < 0)
			return;
	}

	const auto pitch = XMMatrixRotationAxis(XMLoadFloat3(&_right), angle);

	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), pitch));
	XMStoreFloat3(&_at, XMVector3TransformNormal(XMLoadFloat3(&_at), pitch));
}

void Camera::Yaw(const float angle)
{
	const auto yaw = XMMatrixRotationY(angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), yaw));
	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), yaw));
	XMStoreFloat3(&_at, XMVector3TransformNormal(XMLoadFloat3(&_at), yaw));
}

void Camera::Update(const float deltaTime)
{
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	auto r = XMLoadFloat3(&_right);
	auto u = XMLoadFloat3(&_up);
	auto l = XMLoadFloat3(&_at);
	const auto p = XMLoadFloat3(&_eye);

	l = XMVector3Normalize(l); // Normalise the Look vector
	u = XMVector3Normalize(XMVector3Cross(l, r)); // Normalise the Up vector
	r = XMVector3Cross(u, l); // Normalise the Right vector

	const auto x = -XMVectorGetX(XMVector3Dot(p, r));
	const auto y = -XMVectorGetX(XMVector3Dot(p, u));
	const auto z = -XMVectorGetX(XMVector3Dot(p, l));

	XMStoreFloat3(&_right, r);
	XMStoreFloat3(&_up, u);
	XMStoreFloat3(&_at, l);

	_view(0, 0) = _right.x;
	_view(1, 0) = _right.y;
	_view(2, 0) = _right.z;
	_view(3, 0) = x;

	_view(0, 1) = _up.x;
	_view(1, 1) = _up.y;
	_view(2, 1) = _up.z;
	_view(3, 1) = y;

	_view(0, 2) = _at.x;
	_view(1, 2) = _at.y;
	_view(2, 2) = _at.z;
	_view(3, 2) = z;

	_view(0, 3) = 0.0f;
	_view(1, 3) = 0.0f;
	_view(2, 3) = 0.0f;
	_view(3, 3) = 1.0f;
}
