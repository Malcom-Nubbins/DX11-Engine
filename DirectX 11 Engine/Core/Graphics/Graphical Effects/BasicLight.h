#pragma once
#include <vector>
#include "../../Scene/Camera.h"
#include "../../Scene/Scene Elements/SceneElement.h"
#include "Shadows.h"
#include "../../Handlers/SystemHandlers.h"


class BasicLight
{
private:
	ID3D11VertexShader* _lightVS;
	ID3D11HullShader* _tesselationHS;
	ID3D11DomainShader* _tesselationDS;
	ID3D11PixelShader*	_lightPS;
	ID3D11InputLayout*	_simpleVertexInputLayout;

	ID3D11Texture2D*	m_RenderTargetTex2D;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11ShaderResourceView* m_RenderTargetShaderResourceView;

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

	UINT m_MSAACount;
	bool _renderWireframe;

public:
	BasicLight();
	~BasicLight();
	void Cleanup() const;

	void PreResize();
	void Resize(float newWidth, float newHeight);

	HRESULT Initialise(float windowWidth, float windowHeight);

	void SetAsCurrentShader();
	void SetAsCurrentRenderTarget();
	void SetAsCurrentViewport();

	void SetWireframeMode(bool state) { _renderWireframe = state; }
	bool GetWireframeState() const { return _renderWireframe; }

	void CalculateLightColour(DirectionalLight& sceneLight, float sunHeight, FogValuesBuffer& sceneFog);
	ID3D11ShaderResourceView* GetRenderTargetSRV() const { return m_RenderTargetShaderResourceView; }

	void Render(Camera& camera, 
		const DirectionalLight& sceneLight, const std::vector<PointLight>& pointLights, const SpotLight& spotLight, 
		const FogValuesBuffer& fogValues, const std::vector<SceneElement*>& sceneElements, Shadows& shadowClass);

	void Render(Camera& camera, const DirectionalLight& sceneLight, const SpotLight& spotLight, const::std::vector<SceneElement*>& sceneElements, Shadows& shadowClass);

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseRenderTargetAndDepthStencilViews(float windowWidth, float windowHeight);
	void InitialiseViewport(float windowWidth, float windowHeight);
};

