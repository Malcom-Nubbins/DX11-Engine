#pragma once

#include "../../Globals/Structs.h"

class HeatHaze
{
private:
	ID3D11PixelShader*	m_HeatHazePS;
	ID3D11InputLayout*	m_InputLayout;

	ID3D11Texture2D*	m_RenderTargetTex2D;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11ShaderResourceView* m_RenderTargetSRV;

	ID3D11ShaderResourceView* m_SnowTex;
	ID3D11ShaderResourceView* m_HeatTex;

	ObjectMesh		m_Quad;

	ID3D11Buffer* m_HeatHazeValuesBufferPtr;
	HeatHazeValues m_HeatHazeValues;

public:
	HeatHaze();
	~HeatHaze();
	void Cleanup() const;

	HRESULT Initialise(float width, float height);
	void PreResize();
	void Resize(float width, float height);

	void SetAsCurrentRenderTarget() const;

	void SetAsCurrentPixelShader() const;
	ID3D11ShaderResourceView* GetTexture() const { return m_RenderTargetSRV; }

	void Update(float deltaTime);
	void Render(ID3D11ShaderResourceView* textureToProcess, std::string season);

private:
	HRESULT InitialiseRenderTarget(float width, float height);
	HRESULT InitialiseShaders();
	void BuildQuad();
};

