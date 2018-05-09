#pragma once
#include <vector>
#include <algorithm>

#include "../../Handlers/D3DClass.h"
#include "../../Handlers/RenderClass.h"
#include "../../Handlers/ShaderClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Handlers/WindowClass.h"

#include "Shadows.h"

#include "../../Globals/Structs.h"
#include "../Scene Elements\SceneElement.h"
#include "../Camera.h"

#include "../../Loaders/Terrain Generation/DiamondSquareTerrain.h"

class BasicLight
{
private:
	D3DClass * _d3dClass;
	ShaderClass* _shaderClass;
	RenderClass* _renderClass;
	BufferClass* _bufferClass;

	ID3D11RasterizerState* _cwCullMode;
	ID3D11VertexShader* _lightVS;
	ID3D11PixelShader*	_lightPS;
	ID3D11InputLayout*	_simpleVertexInputLayout;

	ID3D11Texture2D*	_renderTargetTex2D;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11ShaderResourceView* _renderTargetSRV;

	ID3D11DepthStencilState* _dsLessEqual;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	D3D11_VIEWPORT _viewport;

	ID3D11Buffer*	_fogValuesBuffer;

	XMFLOAT4 _lightColourDayDiffuse;
	XMFLOAT4 _lightColourDayAmbient;
	XMFLOAT4 _lightColourDaySpecular;

	XMFLOAT4 _lightColourSunsetDiffuse;
	XMFLOAT4 _lightColourSunsetAmbient;
	XMFLOAT4 _lightColourSunsetSpecular;

public:
	BasicLight(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass);
	~BasicLight();
	void Cleanup();

	void Resize(float newWidth, float newHeight);

	HRESULT Initialise(float windowWidth, float windowHeight);

	void SetAsCurrentShader();
	void SetAsCurrentRenderTarget();
	void SetAsCurrentViewport();
	void SetDepthEnabled();

	void CalculateLightColour(DirectionalLight & sceneLight, float sunHeight, FogValuesBuffer & sceneFog);
	ID3D11ShaderResourceView* GetRenderTargetSRV() const { return _renderTargetSRV; }

	void Render(const Camera& camera, 
		const DirectionalLight& sceneLight, const std::vector<PointLight>& pointLights, const SpotLight& spotLight, 
		const FogValuesBuffer& fogValues, const std::vector<SceneElement*>& sceneElements, DiamondSquareTerrain& terrain, ID3D11Buffer* matrixBuffer, ID3D11Buffer* objectValueBuffer, Shadows& shadowClass);

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseRenderTargetAndDepthStencilViews(float windowWidth, float windowHeight);
	void InitialiseViewport(float windowWidth, float windowHeight);
};

