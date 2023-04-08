#pragma once

#include "../../Globals/Structs.h"

class RenderToFullscreenQuad
{
private:
	ID3D11VertexShader* m_QuadVS;
	ID3D11PixelShader* m_QuadPS;

	ID3D11RenderTargetView* m_MSAARTV;
	ID3D11Texture2D* m_MSAARenderTargetTex2D;

	ID3D11InputLayout* m_InputLayout;
	ID3D11Buffer* m_ValuesBufferPtr;

	ObjectMesh		m_Quad;

	UINT m_MSAACount;

public:
	RenderToFullscreenQuad();
	~RenderToFullscreenQuad();
	void Cleanup();

	HRESULT Initialise(float width, float height);
	HRESULT InitRenderTargets(float width, float height);

	void PreResize();
	void OnResize(float width, float height);

	void SetAsCurrentRenderTarget() const;

	void SetAsCurrentVertexShader() const;
	void SetAsCurrentPixelShader() const;

	void Render(ID3D11ShaderResourceView* textureToRender);

private:
	HRESULT InitialiseShaders();
	void BuildQuad();
};

