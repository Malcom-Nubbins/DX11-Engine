#pragma once
#include "../../Handlers/D3DClass.h"
#include "../../Handlers/WindowClass.h"
#include "../../Handlers/RenderClass.h"
#include "../../Handlers/ShaderClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Scene/Camera.h"
#include "UIBitmap.h"

class UserInterface
{
private:
	D3DClass* _d3dClass;
	RenderClass* _renderClass;
	ShaderClass* _shaderClass;
	BufferClass* _bufferClass;
	WindowClass* _windowClass;

	Camera* _camera;
	std::vector<UIBitmap*> _bitmaps;

	ID3D11InputLayout* _inputLayout;
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;

	ID3D11Buffer* _matrixBuffer;
	XMFLOAT4X4 _worldMatrix;

public:
	UserInterface(D3DClass* d3dClass, ShaderClass* shaderClass, RenderClass* renderClass,
		BufferClass* bufferClass, WindowClass* windowClass, Camera* camera);
	~UserInterface();

	void Initialise();

	void AddBitmapToUI(XMFLOAT2 bitmapSize, XMFLOAT2 bitmapPos, ID3D11ShaderResourceView* bitmapTexture);

	void Update(float delta);
	void Draw();
};
