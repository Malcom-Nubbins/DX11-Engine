#pragma once
#include "../../../Globals/stdafx.h"
#include "../../../Globals/Structs.h"

class Transform
{
private:
	XMFLOAT4X4 m_World;
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Rotation;
	XMFLOAT3 m_Scale;

	XMFLOAT3 m_PrevPosition;

	Transform* m_Parent;

public:
	Transform();
	~Transform();

	void SetPosition(XMFLOAT3 pos) { m_Position = pos; }
	XMFLOAT3 GetPosition() const { return m_Position; }

	void SetRotation(XMFLOAT3 rot) { m_Rotation = rot; }
	XMFLOAT3 GetRotation() const { return m_Rotation; }

	void SetScale(XMFLOAT3 scale) { m_Scale = scale; }
	XMFLOAT3 GetScale() const { return m_Scale; }

	XMFLOAT4X4& GetWorld() { return m_World; }

	void SetParent(Transform* parent) { m_Parent = parent; }

	void Update(double delta);
};

