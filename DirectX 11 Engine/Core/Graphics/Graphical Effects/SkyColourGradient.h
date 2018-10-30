#pragma once
#include "../../Scene/Scene Elements/SceneElement.h"
#include "../../Scene/Camera.h"
#include "../../Handlers/SystemHandlers.h"

class SkyColourGradient
{
private:
	SystemHandlers* _systemHandlers;
	ID3D11VertexShader* _colourGradientVS;
	ID3D11PixelShader* _colourGradientPS;
	ID3D11InputLayout* _inputLayout;

	XMFLOAT3 _sceneCentre;
	SceneElement* _skyDomeElement;

	ID3D11Buffer* _gradientValuesBuffer;
	ID3D11Buffer* _matrixBuffer;

public:
	SkyColourGradient(SystemHandlers* system);
	~SkyColourGradient();
	void Cleanup();

	HRESULT Initialise();

	void SetSceneCentre(XMFLOAT3 pos) { _sceneCentre = pos; }

	void SetAsCurrentShader();

	void Update(float deltaTime);
	void Render(const Camera& camera, const XMFLOAT3& sunPos);

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseBuffers();
	void InitialiseSkydomeElement();
};

