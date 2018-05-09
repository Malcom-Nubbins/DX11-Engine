#pragma once
#include <vector>

#include "../../Handlers/D3DClass.h"
#include "../../Handlers/RenderClass.h"
#include "../../Handlers/ShaderClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Handlers/WindowClass.h"

#include "../../Globals/Structs.h"
#include "../Scene Elements\SceneElement.h"
#include "../Camera.h"

#include "../../Loaders/Terrain Generation/DiamondSquareTerrain.h"

class Shadows
{
	struct BoundingSphere
	{
		BoundingSphere() : sphereRadius(0.0f), sphereCentre(0.0f, 0.0f, 0.0f) {}
		XMFLOAT3 sphereCentre;
		float sphereRadius;
	};

private:
	D3DClass * _d3dClass;
	ShaderClass* _shaderClass;
	RenderClass* _renderClass;
	BufferClass* _bufferClass;

	ID3D11RasterizerState* _shadowRasterizer;
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
	Shadows(D3DClass * d3dClass, RenderClass * renderClass, ShaderClass * shaderClass, BufferClass * bufferClass);
	~Shadows();
	void Cleanup();

	HRESULT Initialise(float windowWidth, float windowHeight);

	void SetAsCurrentShader();
	void SetAsCurrentDepthStencil();
	void SetAsCurrentViewport();

	void UpdateLightDirection(XMFLOAT3 lightDirection);
	void BuildShadowTransform();

	void SetSceneCentre(XMFLOAT3 newCentre);

	void Render(const std::vector<SceneElement*>& sceneElements, DiamondSquareTerrain& terrain);

	XMFLOAT4X4 GetShadowTransform() const { return _shadowTransformMatrix; }
	ID3D11ShaderResourceView* GetShadowSRV() const { return _ShadowSRV; }
	XMFLOAT3 GetLightPosition() const { return _lightPosition; }

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseDepthStencilView(float windowWidth, float windowHeight);
	void InitialiseViewport(float windowWidth, float windowHeight);

private:

};

