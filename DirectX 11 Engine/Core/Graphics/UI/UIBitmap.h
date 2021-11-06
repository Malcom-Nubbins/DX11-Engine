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

	XMFLOAT2 m_DynamicPos;

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

	void SetIsDynamicPos(bool const dynamic) { m_IsPositionDynamic = dynamic; }
	void SetDynamicPos(XMFLOAT2 const pos);

	void UpdateScreenSize(XMFLOAT2 newScreenSize);

	SceneElement* GetUIElement() { return _uiElement; }

	void SetShouldDraw(bool const draw) { m_ShouldDraw = draw; }

	void Update(double delta);
	void Draw();

	StringHash GetName() const { return _uiElement->GetElementName(); }

private:
	void UpdateBuffers(XMFLOAT4 const& inVertsPos);
	XMFLOAT4 CalculateFinalPosition();

	bool m_ShouldDraw;
	bool m_IsPositionDynamic;
};
