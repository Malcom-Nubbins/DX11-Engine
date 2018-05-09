#include "Camera.h"

Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::SetLens(float fov, float aspect, float nearZ, float farZ)
{
	_fovY = fov;
	_aspect = aspect;
	_nearZ = nearZ;
	_farZ = farZ;

	XMMATRIX proj = XMMatrixPerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
	XMStoreFloat4x4(&_projection, proj);
}

void Camera::LookAt(XMFLOAT3 right, XMFLOAT3 at, XMFLOAT3 up)
{
	_right = right;
	_at = at;
	_up = up;

	XMVECTOR Right = XMLoadFloat3(&_right);
	XMVECTOR At = XMLoadFloat3(&_at);
	XMVECTOR Up = XMLoadFloat3(&_up);

	XMStoreFloat4x4(&_view, XMMatrixLookToLH(Right, At, Up));
}

void Camera::SetPosition(XMFLOAT3 position)
{
	_eye = position;
}

void Camera::Strafe(float amount)
{
	XMVECTOR strafe = XMVectorReplicate(amount);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMStoreFloat3(&_eye, XMVectorMultiplyAdd(strafe, right, eye));
}

void Camera::Walk(float amount)
{
	XMVECTOR walk = XMVectorReplicate(amount);
	XMVECTOR at = XMLoadFloat3(&_at);
	XMVECTOR eye = XMLoadFloat3(&_eye);
	XMStoreFloat3(&_eye, XMVectorMultiplyAdd(walk, at, eye));
}

void Camera::Pitch(float angle)
{
	XMMATRIX pitch = XMMatrixRotationAxis(XMLoadFloat3(&_right), angle);

	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), pitch));
	XMStoreFloat3(&_at, XMVector3TransformNormal(XMLoadFloat3(&_at), pitch));
}

void Camera::Yaw(float angle)
{
	XMMATRIX yaw = XMMatrixRotationY(angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), yaw));
	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), yaw));
	XMStoreFloat3(&_at, XMVector3TransformNormal(XMLoadFloat3(&_at), yaw));
}

void Camera::Update(float deltaTime)
{
	float movementSpeed = 1.0f;
	if (GetAsyncKeyState('A'))
	{
		Strafe(-movementSpeed * deltaTime);
	}

	if (GetAsyncKeyState('D'))
	{
		Strafe(movementSpeed * deltaTime);
	}
	if (GetAsyncKeyState('W'))
	{
		Walk(movementSpeed * deltaTime);
	}

	if (GetAsyncKeyState('S'))
	{
		Walk(-movementSpeed * deltaTime);
	}

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&_right);
	XMVECTOR U = XMLoadFloat3(&_up);
	XMVECTOR L = XMLoadFloat3(&_at);
	XMVECTOR P = XMLoadFloat3(&_eye);

	L = XMVector3Normalize(L); // Normalise the Look vector
	U = XMVector3Normalize(XMVector3Cross(L, R)); // Normalise the Up vector
	R = XMVector3Cross(U, L); // Normalise the Right vector

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&_right, R);
	XMStoreFloat3(&_up, U);
	XMStoreFloat3(&_at, L);

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
