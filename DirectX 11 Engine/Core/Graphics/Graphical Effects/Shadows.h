#pragma once
#include <vector>

#include "../../Scene/Scene Elements/SceneElement.h"
#include "../../Loaders/Terrain Generation/DiamondSquareTerrain.h"

class Shadows
{
	struct BoundingSphere
	{
		BoundingSphere() : SphereCentre(0.0f, 0.0f, 0.0f), SphereRadius(0.0f) {}
		XMFLOAT3 SphereCentre;
		float SphereRadius;
	};

private:
	ID3D11VertexShader* m_ShadowDepthVS;
	ID3D11InputLayout* m_InputLayout;

	ID3D11Texture2D* m_ShadowTex = nullptr;
	ID3D11DepthStencilView* m_ShadowDepthStencilView = nullptr;
	ID3D11ShaderResourceView* m_ShadowSRV = nullptr;
	D3D11_VIEWPORT m_ShadowViewport;

	BoundingSphere m_SceneBoundary;
	XMFLOAT3 m_LightDirection;
	XMFLOAT3 m_LightPosition;

	XMFLOAT4X4 m_LightViewMatrix;
	XMFLOAT4X4 m_LightProjectionMatrix;
	XMFLOAT4X4 m_ShadowTransformMatrix;

	ID3D11Buffer* m_ShadowDepthMatrixBufferPtr;

public:
	Shadows();
	~Shadows();
	void Cleanup();

	HRESULT Initialise(float windowWidth, float windowHeight);

	void PreResizeViews();
	void OnResize(float windowWidth, float windowHeight);

	void SetAsCurrentShader();
	void SetAsCurrentDepthStencil();
	void SetAsCurrentViewport();

	void UpdateLightDirection(XMFLOAT3 lightDirection);
	void BuildShadowTransform();

	void SetSceneCentre(XMFLOAT3 newCentre);

	void Render(const std::vector<SceneElement*>& sceneElements);

	XMFLOAT4X4& GetShadowTransform() { return m_ShadowTransformMatrix; }
	ID3D11ShaderResourceView* GetShadowSRV() const { return m_ShadowSRV; }
	XMFLOAT3 GetLightPosition() const { return m_LightPosition; }

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseDepthStencilView(float windowWidth, float windowHeight);
	void InitialiseViewport(float windowWidth, float windowHeight);

private:

};

