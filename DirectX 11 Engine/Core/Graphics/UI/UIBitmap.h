#pragma once
#include "../../Globals/Structs.h"
#include "../../Handlers/System Handlers/D3DClass.h"
#include "../../Handlers/System Handlers/BufferClass.h"
#include "../../Scene/Scene Elements/SceneElement.h"

class UIBitmap
{
private:
	XMFLOAT2 _screenSize;
	XMFLOAT2 _bitmapSize;
	XMFLOAT2 _bitmapPrevPosition;
	ID3D11ShaderResourceView* _texture;

	VertexBuffer _vertexBuffer;
	IndexBuffer _indexBuffer;

	ObjectMesh	_uiQuad;
	SceneElement* _uiElement;

	UIOriginPoint m_Origin;
public:

	UIBitmap();
	~UIBitmap();

	void Cleanup();

	void Initialise(XMFLOAT2 const screenSize, XMFLOAT2 const bitmapSize, UIOriginPoint const anchorPoint, ID3D11ShaderResourceView* const texture);

	void UpdateScreenSize(XMFLOAT2 newScreenSize);
	void MoveBitmap(XMFLOAT2 newPos);

	SceneElement* GetUIElement() { return _uiElement; }

	void Update(double delta);
	void Draw();

private:
	void UpdateBuffers(XMFLOAT4 const& inVertsPos);
	XMFLOAT4 CalculatePosition();

};
