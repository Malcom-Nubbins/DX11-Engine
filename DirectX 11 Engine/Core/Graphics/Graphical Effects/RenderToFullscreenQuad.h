#pragma once

#include "../../Globals/Structs.h"
#include "../../Handlers/SystemHandlers.h"

class RenderToFullscreenQuad
{
private:
	SystemHandlers* _systemHandlers;
	ID3D11RenderTargetView* _backBuffer;

	ID3D11VertexShader* _quadVS;
	ID3D11PixelShader* _quadPS;

	ID3D11InputLayout* _inputLayout;
	ObjectMesh		_quad;
	ID3D11Buffer*	_quadVertexBuffer;
	ID3D11Buffer*	_quadIndexBuffer;

public:
	RenderToFullscreenQuad(SystemHandlers* systemHandlers);
	~RenderToFullscreenQuad();
	void Cleanup();

	HRESULT Initialise(float width, float height);

	void Resize(float width, float height);

	void SetAsCurrentRenderTarget();

	void SetAsCurrentVertexShader();
	void SetAsCurrentPixelShader();

	void Render(ID3D11ShaderResourceView* textureToRender);

private:
	HRESULT InitialiseBackBuffer(float width, float height);
	HRESULT InitialiseShaders();
	void BuildQuad();
};

