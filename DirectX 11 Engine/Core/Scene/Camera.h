#pragma once
#include "../Globals/stdafx.h"

using namespace DirectX;

class Camera
{
private:

	XMFLOAT3 m_Eye;

	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up; 
	XMFLOAT3 m_At; 

	float m_Near;
	float m_Far;
	float m_Aspect;
	float m_FovY;

	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_DefaultView;

	XMFLOAT4X4 m_PerspectiveProj;
	XMFLOAT4X4 m_OthographicProj;

	XMFLOAT4X4 m_ViewProjection;

	bool m_OrthographicMode;
public:
	Camera(XMFLOAT3 eye, XMFLOAT3 right, XMFLOAT3 up, XMFLOAT3 at, 
		float nearZ, float farZ, float aspect, float fov, bool orthographic = false);
	~Camera();

	XMFLOAT4X4& GetView() { return m_View; }
	XMFLOAT4X4& GetDefaultView() { return m_DefaultView; }
	XMFLOAT4X4& GetPerspectiveProj() { return m_PerspectiveProj; }
	XMFLOAT4X4& GetOthographicProj() { return m_OthographicProj; }
	XMFLOAT4X4& GetViewProjection() { return m_ViewProjection; }
	XMFLOAT3 GetPosition() const { return m_Eye; }
	XMFLOAT3 GetLookDirection()const { return m_At; }

	void SetPerspective(bool usePerspective);
	void SetLens();
	void LookAt();
	void SetPosition(XMFLOAT3 position);

	void SetViewProjection();

	void SetFOV(float fov);
	void Reset(float fov, float nearZ, float farZ, float aspect);

	void Strafe(double const amount);
	void Walk(double const amount);

	void Pitch(float const angle);
	void Yaw(float const angle);

	void Update(float deltaTime);

private:
	void UpdateViewMatrix();
};

