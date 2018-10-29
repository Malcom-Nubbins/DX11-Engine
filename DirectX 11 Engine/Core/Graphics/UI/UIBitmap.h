#pragma once
#include "../../Globals/Structs.h"
#include "../../Handlers/D3DClass.h"
#include "../../Handlers/BufferClass.h"
#include "../../Scene/Scene Elements/SceneElement.h"

class UIBitmap
{
private:
	D3DClass* _d3dClass;
	BufferClass* _bufferClass;

	XMFLOAT2 _screenSize;
	XMFLOAT2 _bitmapSize;
	XMFLOAT2 _bitmapPrevPosition;
	ID3D11ShaderResourceView* _texture;

	VertexBuffer* _vertexBuffer;
	IndexBuffer* _indexBuffer;

	ObjectMesh	_uiQuad;
	SceneElement* _uiElement;
public:

	UIBitmap(D3DClass* d3dClass, BufferClass* bufferClass);
	~UIBitmap();

	void Initialise(XMFLOAT2 screenSize, XMFLOAT2 bitmapSize, ID3D11ShaderResourceView* texture);
	void MoveBitmap(XMFLOAT2 newPos);

	SceneElement* GetUIElement() { return _uiElement; }

	void Update(float delta);
	void Draw();

};
