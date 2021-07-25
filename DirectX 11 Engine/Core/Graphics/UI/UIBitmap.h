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
	XMFLOAT2 m_Offset;

	ID3D11ShaderResourceView* _texture;

	ObjectMesh	_uiQuad;
	SceneElement* _uiElement;

	UIAnchorPoint m_Anchor;
	UIOriginPoint m_Origin;

	std::string m_ElementName;

public:

	UIBitmap();
	~UIBitmap();

	void Cleanup();

	void Initialise(XMFLOAT2 const screenSize, S_UIElementInfo const inElementInfo);
	void SetPosition();

	void UpdateScreenSize(XMFLOAT2 newScreenSize);

	SceneElement* GetUIElement() { return _uiElement; }

	void Update(double delta);
	void Draw();

	std::string GetName() const { return m_ElementName; }

private:
	void UpdateBuffers(XMFLOAT4 const& inVertsPos);
	XMFLOAT4 CalculateFinalPosition();

};
