#pragma once
#include "../../Handlers/D3DClass.h"
#include "../../Handlers/RenderClass.h"
#include "../../Handlers/ShaderClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Loaders/OBJLoader.h"
#include "../../Scene/Scene Elements/SceneElement.h"
#include "../../Scene/Camera.h"

class SkyColourGradient
{
private:
	D3DClass * _d3dClass;
	ShaderClass* _shaderClass;
	RenderClass* _renderClass;
	BufferClass* _bufferClass;

	ID3D11VertexShader* _colourGradientVS;
	ID3D11PixelShader* _colourGradientPS;
	ID3D11InputLayout* _inputLayout;

	XMFLOAT3 _sceneCentre;
	SceneElement* _skyDomeElement;

	ID3D11Buffer* _gradientValuesBuffer;
	ID3D11Buffer* _matrixBuffer;

public:
	SkyColourGradient(D3DClass * d3dClass, RenderClass * renderClass, ShaderClass * shaderClass, BufferClass * bufferClass);
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
