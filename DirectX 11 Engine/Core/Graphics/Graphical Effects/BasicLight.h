#pragma once
#include <vector>
#include "../../Scene/Camera.h"
#include "../../Scene/Scene Elements/SceneElement.h"
#include "Shadows.h"
#include "../../Handlers/SystemHandlers.h"


class BasicLight
{
private:
	const SystemHandlers& _systemHandlers;

	ID3D11VertexShader* _lightVS;
	ID3D11HullShader* _tesselationHS;
	ID3D11DomainShader* _tesselationDS;
	ID3D11PixelShader*	_lightPS;
	ID3D11InputLayout*	_simpleVertexInputLayout;

	ID3D11Texture2D*	_renderTargetTex2D;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11ShaderResourceView* _renderTargetSRV;

	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView* _depthStencilView;
	D3D11_VIEWPORT _viewport;

	ID3D11Buffer*	_fogValuesBuffer;
	ID3D11Buffer* _tesselationBuffer;
	ID3D11Buffer* _camLightBuffer;

	ID3D11Buffer* _matrixBuffer;
	ID3D11Buffer* _objectValueBuffer;

	XMFLOAT4 _lightColourDayDiffuse;
	XMFLOAT4 _lightColourDayAmbient;
	XMFLOAT4 _lightColourDaySpecular;

	XMFLOAT4 _lightColourSunsetDiffuse;
	XMFLOAT4 _lightColourSunsetAmbient;
	XMFLOAT4 _lightColourSunsetSpecular;

	XMFLOAT4 _lightColourNightDiffuse;
	XMFLOAT4 _lightColourNightAmbient;
	XMFLOAT4 _lightColourNightSpecular;

	bool _renderWireframe;

public:
	BasicLight(const SystemHandlers& systemHandlers);
	~BasicLight();
	void Cleanup() const;

	void Resize(float newWidth, float newHeight);

	HRESULT Initialise(float windowWidth, float windowHeight);

	void SetAsCurrentShader();
	void SetAsCurrentRenderTarget();
	void SetAsCurrentViewport();

	void SetWireframeMode(bool state) { _renderWireframe = state; }
	bool GetWireframeState() const { return _renderWireframe; }

	void CalculateLightColour(DirectionalLight& sceneLight, float sunHeight, FogValuesBuffer& sceneFog);
	ID3D11ShaderResourceView* GetRenderTargetSRV() const { return _renderTargetSRV; }

	void Render(const Camera& camera, 
		const DirectionalLight& sceneLight, const std::vector<PointLight>& pointLights, const SpotLight& spotLight, 
		const FogValuesBuffer& fogValues, const std::vector<SceneElement*>& sceneElements, Shadows& shadowClass);

	void Render(const Camera&, const DirectionalLight& sceneLight, const SpotLight& spotLight, const::std::vector<SceneElement*>& sceneElements, Shadows& shadowClass);

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseRenderTargetAndDepthStencilViews(float windowWidth, float windowHeight);
	void InitialiseViewport(float windowWidth, float windowHeight);
};

