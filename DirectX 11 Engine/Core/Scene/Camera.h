#pragma once
#include "../Globals/stdafx.h"
#include "../Handlers/WindowClass.h"

using namespace DirectX;

class Camera
{
private:
	WindowClass* _windowClass;

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
	XMFLOAT4X4 _defaultView;

	XMFLOAT4X4 _perspectiveProj;
	XMFLOAT4X4 _othographicProj;

	bool _orthographicMode;
public:
	Camera(WindowClass* windowClass);
	Camera(const Camera& copy);
	~Camera();

	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetDefaultView() const { return _defaultView; }
	XMFLOAT4X4 GetPerspectiveProj() const { return _perspectiveProj; }
	XMFLOAT4X4 GetOthographicProj() const { return _othographicProj; }
	XMFLOAT3 GetPosition() const { return _eye; }
	XMFLOAT3 GetLookDirection()const { return _at; }

	void SetPerspective(bool usePerspective);
	void SetLens(float fov, float width, float height, float nearZ, float farZ);
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

