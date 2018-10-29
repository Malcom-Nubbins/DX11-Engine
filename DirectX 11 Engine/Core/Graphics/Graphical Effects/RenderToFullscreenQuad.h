#pragma once
#include "../../Handlers/D3DClass.h"
#include "../../Handlers/RenderClass.h"
#include "../../Handlers/ShaderClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Handlers/WindowClass.h"

#include "../../Globals/Structs.h"
class RenderToFullscreenQuad
{
private:
	D3DClass * _d3dClass;
	ShaderClass* _shaderClass;
	RenderClass* _renderClass;
	BufferClass* _bufferClass;

	ID3D11RenderTargetView* _backBuffer;

	ID3D11VertexShader* _quadVS;
	ID3D11PixelShader* _quadPS;

	ID3D11InputLayout* _inputLayout;
	ObjectMesh		_quad;
	ID3D11Buffer*	_quadVertexBuffer;
	ID3D11Buffer*	_quadIndexBuffer;

public:
	RenderToFullscreenQuad(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass, BufferClass* bufferClass);
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

