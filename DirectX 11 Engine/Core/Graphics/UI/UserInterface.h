#pragma once
#include "../../Scene/Camera.h"
#include "UIBitmap.h"
#include "../../Handlers/SystemHandlers.h"

class UserInterface
{
private:
	Camera& _camera;
	std::vector<UIBitmap*> _bitmaps;

	ID3D11InputLayout* _inputLayout;
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;

	ID3D11Buffer* _matrixBuffer;
	XMFLOAT4X4 _worldMatrix;

public:
	UserInterface(Camera& camera);
	~UserInterface();

	void Cleanup();

	void Resize(float width, float height);

	void Initialise();

	void AddBitmapToUI(XMFLOAT2 const bitmapSize, UIAnchorPoint const anchorPoint, UIOriginPoint const originPoint, ID3D11ShaderResourceView* const bitmapTexture);

	void Update(double delta);
	void Draw();
};
