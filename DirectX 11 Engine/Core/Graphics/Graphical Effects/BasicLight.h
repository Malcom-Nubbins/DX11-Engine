#pragma once
#include <vector>
#include "../../Scene/Camera.h"
#include "../../Scene/Scene Elements/SceneElement.h"
#include "Shadows.h"

class BasicLight
{
private:
	ID3D11VertexShader*			m_LightVS;
	ID3D11VertexShader*			m_TesselationVS;
	ID3D11HullShader*			m_TesselationHS;
	ID3D11DomainShader*			m_TesselationDS;
	ID3D11PixelShader*			m_LightPS;
	ID3D11InputLayout*			m_SimpleVertexInputLayout;

	ID3D11Texture2D*			m_RenderTargetTex2D;
	ID3D11RenderTargetView*		m_RenderTargetView;
	ID3D11ShaderResourceView*	m_RenderTargetShaderResourceView;

	ID3D11Texture2D*			m_DepthStencilBuffer;
	ID3D11DepthStencilView*		m_DepthStencilView;
	D3D11_VIEWPORT				m_Viewport;

	ID3D11Buffer*				m_FogValuesBufferPtr;
	ID3D11Buffer*				m_TesselationBufferPtr;
	ID3D11Buffer*				m_CamLightBufferPtr;

	ID3D11Buffer*				m_MatrixBufferPtr;
	ID3D11Buffer*				m_ObjectValueBufferPtr;

	XMFLOAT4					m_LightColourDayDiffuse = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4					m_LightColourDayAmbient = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4					m_LightColourDaySpecular = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	XMFLOAT4					m_LightColourSunsetDiffuse = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4					m_LightColourSunsetAmbient = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4					m_LightColourSunsetSpecular = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	XMFLOAT4					m_LightColourNightDiffuse = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4					m_LightColourNightAmbient = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4					m_LightColourNightSpecular = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	UINT						m_MSAACount;
	bool						m_bRenderWireframe;

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

	void SetWireframeMode(bool state) { m_bRenderWireframe = state; }
	bool GetWireframeState() const { return m_bRenderWireframe; }

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

