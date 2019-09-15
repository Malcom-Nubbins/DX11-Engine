#pragma once
#include <vector>

#include "../../Scene/Scene Elements/SceneElement.h"
#include "../../Loaders/Terrain Generation/DiamondSquareTerrain.h"
#include "../../Handlers/SystemHandlers.h"

class Shadows
{
	struct BoundingSphere
	{
		BoundingSphere() : sphereCentre(0.0f, 0.0f, 0.0f), sphereRadius(0.0f) {}
		XMFLOAT3 sphereCentre;
		float sphereRadius;
	};

private:
	const SystemHandlers& _systemHandlers;
	ID3D11VertexShader* _shadowDepthVS;
	ID3D11InputLayout* _inputLayout;

	ID3D11Texture2D* _ShadowTex = nullptr;
	ID3D11DepthStencilView* _ShadowDepthStencilView = nullptr;
	ID3D11ShaderResourceView* _ShadowSRV = nullptr;
	D3D11_VIEWPORT _shadowViewport;

	BoundingSphere _sceneBoundary;
	XMFLOAT3 _lightDirection;
	XMFLOAT3 _lightPosition;

	XMFLOAT4X4 _lightViewMatrix;
	XMFLOAT4X4 _lightProjectionMatrix;
	XMFLOAT4X4 _shadowTransformMatrix;

	ID3D11Buffer* _shadowDepthMatrixBuffer;

public:
	Shadows(const SystemHandlers& system);
	~Shadows();
	void Cleanup();

	HRESULT Initialise(float windowWidth, float windowHeight);

	void SetAsCurrentShader();
	void SetAsCurrentDepthStencil();
	void SetAsCurrentViewport();

	void UpdateLightDirection(XMFLOAT3 lightDirection);
	void BuildShadowTransform();

	void SetSceneCentre(XMFLOAT3 newCentre);

	void Render(const std::vector<SceneElement*>& sceneElements);

	XMFLOAT4X4 GetShadowTransform() const { return _shadowTransformMatrix; }
	ID3D11ShaderResourceView* GetShadowSRV() const { return _ShadowSRV; }
	XMFLOAT3 GetLightPosition() const { return _lightPosition; }

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseDepthStencilView(float windowWidth, float windowHeight);
	void InitialiseViewport(float windowWidth, float windowHeight);

private:

};

