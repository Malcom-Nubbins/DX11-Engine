#pragma once
#include "../../../Globals/stdafx.h"
#include "../../../Globals/Structs.h"

class Transform
{
private:
	XMFLOAT4X4 _world;
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	XMFLOAT3 _prevPosition;

	Transform* _parent;

public:
	Transform();
	~Transform();

	void SetPosition(XMFLOAT3 pos) { _position = pos; }
	XMFLOAT3 GetPosition() const { return _position; }

	void SetRotation(XMFLOAT3 rot) { _rotation = rot; }
	XMFLOAT3 GetRotation() const { return _rotation; }

	void SetScale(XMFLOAT3 scale) { _scale = scale; }
	XMFLOAT3 GetScale() const { return _scale; }

	XMFLOAT4X4& GetWorld() { return _world; }

	void SetParent(Transform* parent) { _parent = parent; }

	void Update(double delta);
};

