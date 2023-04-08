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
	, m_bOrthographicMode(orthographic)
{
	SetLens();
	LookAt();
}

Camera::~Camera()
{
}

float Camera::GetFoVX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_Near);
}

float Camera::GetNearWindowWidth() const
{
	return m_Aspect * m_NearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return m_NearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return m_Aspect * m_FarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return m_FarWindowHeight;
}

void Camera::SetPerspective(const bool usePerspective)
{
	m_bOrthographicMode = !usePerspective;
}

void Camera::SetLens()
{
	DX11Engine const& engine = DX11Engine::Get();

	auto proj = XMMatrixOrthographicLH(static_cast<float>(engine.GetClientWidth()), static_cast<float>(engine.GetClientHeight()), m_Near, m_Far);
	XMStoreFloat4x4(&m_OthographicProj, proj);

	const auto camRight = XMLoadFloat3(&m_Right);
	const auto camAt = XMLoadFloat3(&m_At);
	const auto camUp = XMLoadFloat3(&m_Up);

	proj = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far);
	XMStoreFloat4x4(&m_PerspectiveProj, proj);
	//XMStoreFloat4x4(&_defaultView, XMMatrixLookAtLH(camRight, camAt, camUp));

	m_NearWindowHeight = 2.0f * m_Near * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_Far * tanf(0.5f * m_FovY);
}

void Camera::LookAt()
{
	const auto camRight = XMLoadFloat3(&m_Right);
	const auto camAt = XMLoadFloat3(&m_At);
	const auto camUp = XMLoadFloat3(&m_Up);

	XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(camRight, camAt, camUp));
	XMStoreFloat4x4(&m_DefaultView, XMMatrixLookAtLH(camRight, camAt, camUp));
}

void Camera::SetPosition(const XMFLOAT3 Position)
{
	m_Eye = Position;
}

void Camera::SetViewProjection()
{
	XMMATRIX view = XMLoadFloat4x4(&m_View);
	XMMATRIX proj = XMLoadFloat4x4(&m_PerspectiveProj);

	XMMATRIX viewProj = XMMatrixMultiply(proj, view);

	XMStoreFloat4x4(&m_ViewProjection, viewProj);
}

void Camera::SetFOV(float fov)
{
	m_FovY = fov;

	auto proj = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far);
	XMStoreFloat4x4(&m_PerspectiveProj, proj);

	m_NearWindowHeight = 2.0f * m_Near * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_Far * tanf(0.5f * m_FovY);
}

void Camera::Reset(float fov, float nearZ, float farZ, float aspect)
{
	m_FovY = fov;
	m_Near = nearZ;
	m_Far = farZ;
	m_Aspect = aspect;

	SetLens();
}

void Camera::Strafe(double const amount)
{
	const auto strafe = XMVectorReplicate(static_cast<float>(amount));
	const auto right = XMLoadFloat3(&m_Right);
	const auto eye = XMLoadFloat3(&m_Eye);
	XMStoreFloat3(&m_Eye, XMVectorMultiplyAdd(strafe, right, eye));
}

void Camera::Walk(double const amount)
{
	const auto walk = XMVectorReplicate(static_cast<float>(amount));
	const auto at = XMLoadFloat3(&m_At);
	const auto eye = XMLoadFloat3(&m_Eye);
	XMStoreFloat3(&m_Eye, XMVectorMultiplyAdd(walk, at, eye));
}

void Camera::Pitch(float const angle)
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

void Camera::Yaw(float const angle)
{
	const auto yaw = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), yaw));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), yaw));
	XMStoreFloat3(&m_At, XMVector3TransformNormal(XMLoadFloat3(&m_At), yaw));
}

void Camera::ComputeFrustumFromProjection()
{
	static XMVECTOR HomogenousPoints[6] =
	{
		{1.0f, 0.0f, 1.0f, 1.0f}, // right
		{-1.0f, 0.0f, 1.0f, 1.0f}, // left
		{0.0f, 1.0f, 1.0f, 1.0f}, // top
		{0.0f, -1.0f, 1.0f, 1.0f}, // bottom

		{0.0f, 0.0f, 0.0f, 1.0f}, // near
		{0.0f, 0.0f, 1.0f, 1.0f}, // far
	};

	XMMATRIX Proj = m_bOrthographicMode ? XMLoadFloat4x4(&m_OthographicProj) : XMLoadFloat4x4(&m_PerspectiveProj);

	XMVECTOR Determinant;
	XMMATRIX matInverse = XMMatrixInverse(&Determinant, Proj);

	XMVECTOR Points[6];

	for (uint8 idx = 0; idx < 6; ++idx)
	{
		Points[idx] = XMVector4Transform(HomogenousPoints[idx], matInverse);
	}

	m_CurrentFrustum.Origin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_CurrentFrustum.Orientation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	Points[0] = Points[0] * XMVectorReciprocal(XMVectorSplatZ(Points[0]));
	Points[1] = Points[1] * XMVectorReciprocal(XMVectorSplatZ(Points[1]));
	Points[2] = Points[2] * XMVectorReciprocal(XMVectorSplatZ(Points[2]));
	Points[3] = Points[3] * XMVectorReciprocal(XMVectorSplatZ(Points[3]));

	m_CurrentFrustum.RightSlope = XMVectorGetX(Points[0]);
	m_CurrentFrustum.LeftSlope = XMVectorGetX(Points[1]);
	m_CurrentFrustum.TopSlope = XMVectorGetY(Points[2]);
	m_CurrentFrustum.BottomSlope = XMVectorGetY(Points[3]);

	Points[4] = Points[4] * XMVectorReciprocal(XMVectorSplatW(Points[4]));
	Points[5] = Points[5] * XMVectorReciprocal(XMVectorSplatW(Points[5]));

	m_CurrentFrustum.Near = XMVectorGetZ(Points[4]);
	m_CurrentFrustum.Far = XMVectorGetZ(Points[5]);
}

bool Camera::IsPointInFrustum(XMFLOAT3 InPoint)
{
	bool IsPointInFrustum = true;

	XMVECTOR LeftVec = XMPlaneDotCoord(XMLoadFloat(&m_CurrentFrustum.LeftSlope), XMLoadFloat3(&InPoint));
	XMVECTOR RightVec = XMPlaneDotCoord(XMLoadFloat(&m_CurrentFrustum.RightSlope), XMLoadFloat3(&InPoint));
	XMVECTOR TopVec = XMPlaneDotCoord(XMLoadFloat(&m_CurrentFrustum.TopSlope), XMLoadFloat3(&InPoint));
	XMVECTOR BottomVec = XMPlaneDotCoord(XMLoadFloat(&m_CurrentFrustum.BottomSlope), XMLoadFloat3(&InPoint));
	XMVECTOR NearVec = XMPlaneDotCoord(XMLoadFloat(&m_CurrentFrustum.Near), XMLoadFloat3(&InPoint));
	XMVECTOR FarVec = XMPlaneDotCoord(XMLoadFloat(&m_CurrentFrustum.Far), XMLoadFloat3(&InPoint));

	float FrustumArea[6];
	XMStoreFloat(&FrustumArea[0], LeftVec);
	XMStoreFloat(&FrustumArea[1], RightVec);
	XMStoreFloat(&FrustumArea[2], TopVec);
	XMStoreFloat(&FrustumArea[3], BottomVec);
	XMStoreFloat(&FrustumArea[4], NearVec);
	XMStoreFloat(&FrustumArea[5], FarVec);

	for (uint8 idx = 0; idx < 6; ++idx)
	{
		if (FrustumArea[idx] < 0.0f)
		{
			IsPointInFrustum = false;
			break;
		}
	}

	return IsPointInFrustum;;
}

void Camera::Update(float deltaTime)
{
	UpdateViewMatrix();
	SetViewProjection();
	ComputeFrustumFromProjection();
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
