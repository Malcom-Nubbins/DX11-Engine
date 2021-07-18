#pragma once

#include "../../Globals/Structs.h"
#include "../../Handlers/SystemHandlers.h"

class RenderToFullscreenQuad
{
private:
	ID3D11VertexShader* _quadVS;
	ID3D11PixelShader* _quadPS;

	ID3D11RenderTargetView* m_MSAARTV;
	ID3D11Texture2D* m_MSAARenderTargetTex2D;

	ID3D11InputLayout* _inputLayout;
	ObjectMesh		_quad;

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

	void Render(ID3D11ShaderResourceView* textureToRender) const;

private:
	HRESULT InitialiseShaders();
	void BuildQuad();
};

