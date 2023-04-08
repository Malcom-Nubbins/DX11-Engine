#pragma once
#include "../../Scene/Scene Elements/SceneElement.h"
#include "../../Scene/Camera.h"

class SkyColourGradient
{
private:
	ID3D11VertexShader* m_ColourGradientVS;
	ID3D11PixelShader* m_ColourGradientPS;
	ID3D11InputLayout* m_InputLayout;

	ID3D11ShaderResourceView* m_SkySRV;

	XMFLOAT3 m_SceneCentre;
	SceneElement* m_SkyDomeElement;

	ID3D11Buffer* m_GradientValuesBufferPtr;
	ID3D11Buffer* m_MatrixBufferPtr;

public:
	SkyColourGradient();
	~SkyColourGradient();
	void Cleanup() const;

	HRESULT Initialise();

	void SetSceneCentre(XMFLOAT3 pos) { m_SceneCentre = pos; }

	void SetAsCurrentShader();

	void Update(double deltaTime);
	void Render(Camera& camera, XMFLOAT3& sunPos);

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseBuffers();
	void InitialiseSkydomeElement();
};

