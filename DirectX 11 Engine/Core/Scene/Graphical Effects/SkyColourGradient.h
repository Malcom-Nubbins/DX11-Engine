#pragma once
#include "../../Handlers/D3DClass.h"
#include "../../Handlers/RenderClass.h"
#include "../../Handlers/ShaderClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Handlers/WindowClass.h"

#include "../../Globals/Structs.h"
#include "../Scene Elements\SceneElement.h"
#include "../Camera.h"
#include "../../Loaders/OBJLoader.h"

class SkyColourGradient
{
private:
	D3DClass * _d3dClass;
	ShaderClass* _shaderClass;
	RenderClass* _renderClass;
	BufferClass* _bufferClass;

	ID3D11RasterizerState* _noCullRasterizer;
	ID3D11DepthStencilState* _depthDisabled;
	ID3D11VertexShader* _colourGradientVS;
	ID3D11PixelShader* _colourGradientPS;
	ID3D11InputLayout* _inputLayout;

	XMFLOAT3 _sceneCentre;
	SceneElement* _skyDomeElement;

	ID3D11Buffer* _gradientValuesBuffer;

public:
	SkyColourGradient(D3DClass * d3dClass, RenderClass * renderClass, ShaderClass * shaderClass, BufferClass * bufferClass);
	~SkyColourGradient();
	void Cleanup();

	HRESULT Initialise();

	void SetSceneCentre(XMFLOAT3 pos) { _sceneCentre = pos; }

	void SetAsCurrentShader();

	void Update(float deltaTime);
	void Render(ID3D11Buffer* matrixBuffer, const Camera& camera, const XMFLOAT3& sunPos);

private:
	HRESULT InitialiseShaders();
	HRESULT InitialiseBuffers();
	void InitialiseSkydomeElement();
};

