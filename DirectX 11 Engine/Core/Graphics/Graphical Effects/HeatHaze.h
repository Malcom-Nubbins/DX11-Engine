#pragma once

#include "../../Globals/Structs.h"
#include "../../Handlers/SystemHandlers.h"

class HeatHaze
{
private:
	ID3D11PixelShader*	_heatHazePS;
	ID3D11InputLayout*	_inputLayout;

	ID3D11Texture2D*	_renderTargetTex2D;
	ID3D11RenderTargetView* _renderTargetView;
	ID3D11ShaderResourceView* _renderTargetSRV;

	ObjectMesh		_quad;

	ID3D11Buffer* _heatHazeValues;
	HeatHazeValues _values;

public:
	HeatHaze();
	~HeatHaze();
	void Cleanup() const;

	HRESULT Initialise(float width, float height);
	void PreResize();
	void Resize(float width, float height);

	void SetAsCurrentRenderTarget() const;

	void SetAsCurrentPixelShader() const;
	ID3D11ShaderResourceView* GetTexture() const { return _renderTargetSRV; }

	void Update(float deltaTime);
	void Render(ID3D11ShaderResourceView* textureToProcess, std::string season);

private:
	HRESULT InitialiseRenderTarget(float width, float height);
	HRESULT InitialiseShaders();
	void BuildQuad();
};

