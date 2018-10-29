#pragma once

#include "../../Handlers/D3DClass.h"
#include "../../Handlers/RenderClass.h"
#include "../../Handlers/ShaderClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Globals/Structs.h"
#include "../../Handlers/TextureHandler.h"

class HeatHaze
{
private:
	D3DClass * _d3dClass;
	ShaderClass* _shaderClass;
	RenderClass* _renderClass;
	BufferClass* _bufferClass;

	ID3D11PixelShader*	_heatHazePS;
	ID3D11InputLayout*	_inputLayout;

	ID3D11Texture2D*	_renderTargetTex2D;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11ShaderResourceView* _renderTargetSRV;

	ObjectMesh		_quad;

	ID3D11Buffer* _heatHazeValues;
	HeatHazeValues _values;

public:
	HeatHaze(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass);
	~HeatHaze();
	void Cleanup();

	HRESULT Initialise(float width, float height);
	void Resize(float width, float height);

	void SetAsCurrentRenderTarget();

	void SetAsCurrentPixelShader();
	ID3D11ShaderResourceView* GetTexture() const { return _renderTargetSRV; }

	void Update(float deltaTime);
	void Render(ID3D11ShaderResourceView* textureToProcess, TextureHandler* texHandler, std::string season);

private:
	HRESULT InitialiseRenderTarget(float width, float height);
	HRESULT InitialiseShaders();
	void BuildQuad();
};

