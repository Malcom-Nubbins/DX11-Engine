#pragma once
#include "../Globals/stdafx.h"

using namespace DirectX;

class Camera
{
private:
	XMFLOAT3 _eye;

	XMFLOAT3 _right;
	XMFLOAT3 _up; 
	XMFLOAT3 _at; 

	float _nearZ;
	float _farZ;
	float _aspect;
	float _fovY;
	float _nearWindowHeight;
	float _nearWindowWidth;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
public:
	Camera();
	~Camera();

	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetProj() const { return _projection; }
	XMFLOAT3 GetPosition() const { return _eye; }
	XMFLOAT3 GetLookDirection()const { return _at; }

	void SetLens(float fov, float aspect, float nearZ, float farZ);
	void LookAt(XMFLOAT3 right, XMFLOAT3 at, XMFLOAT3 up);
	void SetPosition(XMFLOAT3 position);

	void Strafe(float amount);
	void Walk(float amount);

	void Pitch(float angle);
	void Yaw(float angle);

	void Update(float deltaTime);

private:
	void UpdateViewMatrix();
};

