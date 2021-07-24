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

	XMFLOAT2 m_Position;

	ID3D11ShaderResourceView* _texture;

	VertexBuffer _vertexBuffer;
	IndexBuffer _indexBuffer;

	ObjectMesh	_uiQuad;
	SceneElement* _uiElement;

	UIAnchorPoint m_Anchor;
	UIOriginPoint m_Origin;

public:

	UIBitmap();
	~UIBitmap();

	void Cleanup();

	void Initialise(XMFLOAT2 const screenSize, XMFLOAT2 const bitmapSize, UIAnchorPoint const anchorPoint, UIOriginPoint const originPoint, ID3D11ShaderResourceView* const texture);
	void SetPosition();

	void UpdateScreenSize(XMFLOAT2 newScreenSize);

	SceneElement* GetUIElement() { return _uiElement; }

	void Update(double delta);
	void Draw();

private:
	void UpdateBuffers(XMFLOAT4 const& inVertsPos);
	XMFLOAT4 CalculateFinalPosition();

};
