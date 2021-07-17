#include "Transform.h"

Transform::Transform()
	: _position(XMFLOAT3()), _rotation(XMFLOAT3()), _scale(XMFLOAT3(1.0f, 1.0f, 1.0f)), _parent(nullptr)
{
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
}

Transform::~Transform()
{
}

void Transform::Update(double deltaTime)
{
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	XMMATRIX rotation = XMMatrixRotationX(_rotation.x) * XMMatrixRotationY(_rotation.y) * XMMatrixRotationZ(_rotation.z);
	XMMATRIX translation = XMMatrixTranslation(_position.x, _position.y, _position.z);

	XMStoreFloat4x4(&_world, scale * rotation * translation);

	if (_parent != nullptr)
	{
		XMStoreFloat4x4(&_world, XMLoadFloat4x4(&this->GetWorld()) * XMLoadFloat4x4(&_parent->GetWorld()));
	}
}
