#pragma once
#include "../../Globals/Structs.h"
#include "../../Handlers/System Handlers/BufferClass.h"
#include "../../Scene/Scene Elements/SceneElement.h"

class UIBitmap
{
private:
	XMFLOAT2 m_ScreenSize;
	XMFLOAT2 m_BitmapSize;

	XMFLOAT2 m_BitmapPosition;
	XMFLOAT2 m_BitmapOffset;

	XMFLOAT2 m_DynamicPos;

	ID3D11ShaderResourceView* m_Texture;

	ObjectMesh	m_UIQuad;
	SceneElement* m_UIElement;

	UIAnchorPoint m_Anchor;
	UIOriginPoint m_Origin;

	uint32 m_Order;

	std::string m_ElementName;

public:

	UIBitmap();
	~UIBitmap();

	void Cleanup();

	void Initialise(XMFLOAT2 const screenSize, UIElementInfo const inElementInfo);
	void SetPosition();

	void SetIsDynamicPos(bool const dynamic) { bIsPositionDynamic = dynamic; }
	void SetDynamicPos(XMFLOAT2 const pos);

	void UpdateScreenSize(XMFLOAT2 newScreenSize);

	SceneElement* GetUIElement() { return m_UIElement; }

	void SetShouldDraw(bool const draw) { bShouldDraw = draw; }

	void Update(double delta);
	void Draw();

	TStringHash GetName() const { return m_UIElement->GetElementName(); }

	uint32 GetOrder() const { return m_Order; }

private:
	void UpdateBuffers(XMFLOAT4 const& inVertsPos);
	XMFLOAT4 CalculateFinalPosition();

	bool bShouldDraw;
	bool bIsPositionDynamic;
};
