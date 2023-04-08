#include "Transform.h"

Transform::Transform()
	: m_Position(XMFLOAT3())
	, m_Rotation(XMFLOAT3())
	, m_Scale(XMFLOAT3(1.0f, 1.0f, 1.0f))
	, m_Parent(nullptr)
	, m_PrevPosition(XMFLOAT3(0.f, 0.f, 0.f))
{
	XMStoreFloat4x4(&m_World, XMMatrixIdentity());
}

Transform::~Transform()
{
}

void Transform::Update(double deltaTime)
{
	XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	XMMATRIX rotation = XMMatrixRotationX(m_Rotation.x) * XMMatrixRotationY(m_Rotation.y) * XMMatrixRotationZ(m_Rotation.z);
	XMMATRIX translation = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);

	XMStoreFloat4x4(&m_World, scale * rotation * translation);

	if (m_Parent != nullptr)
	{
		XMStoreFloat4x4(&m_World, XMLoadFloat4x4(&this->GetWorld()) * XMLoadFloat4x4(&m_Parent->GetWorld()));
	}
}
