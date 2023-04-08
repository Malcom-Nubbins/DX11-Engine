#include "UIBitmap.h"

#include "../../ApplicationNew.h"
#include "../../DX11Engine.h"

UIBitmap::UIBitmap()
	: bShouldDraw(true)
	, bIsPositionDynamic(false)
	, m_DynamicPos(0.0f, 0.0f)
	, m_Order(0)
	, m_ScreenSize(0.f, 0.f)
{
}

UIBitmap::~UIBitmap()
{
}

void UIBitmap::Cleanup()
{
	if (m_Texture != nullptr)
	{
		m_Texture->Release();
	}
	
	m_UIElement->Cleanup();
}

void UIBitmap::Initialise(XMFLOAT2 const screenSize, UIElementInfo const inElementInfo)
{
	auto textureHandler = DX11Engine::Get().GetTextureHandler();

	m_ScreenSize = screenSize;
	m_BitmapSize = inElementInfo.Size;
	m_Anchor = inElementInfo.AnchorPoint;
	m_Origin = inElementInfo.OriginPoint;
	m_ElementName = inElementInfo.ElementName;
	m_BitmapOffset = inElementInfo.Offset;
	m_Texture = textureHandler->GetTextureByName(inElementInfo.TextureName.c_str());
	m_Order = inElementInfo.Order;

	BufferClass::CreateQuadDynamic(&m_UIQuad.VertexBuffer, &m_UIQuad.IndexBuffer);

#if defined(_DEBUG)
	char const uiQuadVBName[] = "UI Quad VB";
	char const uiQuadIBName[] = "UI Quad IB";

	m_UIQuad.VertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(uiQuadVBName) - 1, uiQuadVBName);
	m_UIQuad.IndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(uiQuadIBName) - 1, uiQuadIBName);
#endif

	m_UIQuad.NumberOfIndices = 6;
	m_UIQuad.VertexBufferOffset = 0;
	m_UIQuad.vertexBufferStride = sizeof(SimpleQuad);

	auto uiTransform = std::make_shared<Transform>();
	uiTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	uiTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	uiTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto uiAppearance = std::make_shared<Appearance>(m_UIQuad, ObjectMaterial{});

	m_UIElement = new SceneElement(GetStringHash(m_ElementName), *uiTransform, *uiAppearance);

	SetPosition();

	XMFLOAT4 const finalPos = CalculateFinalPosition();
	UpdateBuffers(finalPos);
}

void UIBitmap::SetPosition()
{
	switch (m_Anchor)
	{
	case UIAnchorPoint::TopLeft:
	{
		m_BitmapPosition = XMFLOAT2(0.0f, 0.0f);
	}
	break;
	case UIAnchorPoint::TopMiddle:
	{
		m_BitmapPosition = XMFLOAT2(m_ScreenSize.x / 2.0f, 0.0f);
	}
	break;
	case UIAnchorPoint::TopRight:
	{
		m_BitmapPosition = XMFLOAT2(m_ScreenSize.x, 0.0f);
	}
	break;
	case UIAnchorPoint::CentreLeft:
	{
		m_BitmapPosition = XMFLOAT2(0.0f, m_ScreenSize.y / 2.0f);
	}
	break;
	case UIAnchorPoint::Centre:
	{
		m_BitmapPosition = XMFLOAT2(m_ScreenSize.x / 2.0f, m_ScreenSize.y / 2.0f);
	}
	break;
	case UIAnchorPoint::CentreRight:
	{
		m_BitmapPosition = XMFLOAT2(m_ScreenSize.x, m_ScreenSize.y / 2.0f);
	}
	break;
	case UIAnchorPoint::BottomLeft:
	{
		m_BitmapPosition = XMFLOAT2(0.0f, m_ScreenSize.y);
	}
	break;
	case UIAnchorPoint::BottomCentre:
	{
		m_BitmapPosition = XMFLOAT2(m_ScreenSize.x / 2.0f, m_ScreenSize.y);
	}
	break;
	case UIAnchorPoint::BottomRight:
	{
		m_BitmapPosition = XMFLOAT2(m_ScreenSize.x, m_ScreenSize.y);
	}
	break;
	default:
		break;
	}
}

void UIBitmap::SetDynamicPos(XMFLOAT2 const pos)
{
	m_DynamicPos = pos;
}

void UIBitmap::UpdateScreenSize(XMFLOAT2 newScreenSize)
{
	if (newScreenSize.x == m_ScreenSize.x && newScreenSize.y == m_ScreenSize.y)
	{
		return;
	}

	m_ScreenSize = newScreenSize;

	SetPosition();

	XMFLOAT4 const bitmapPos = CalculateFinalPosition();

	UpdateBuffers(bitmapPos);
}

void UIBitmap::Update(double delta)
{
	if (bIsPositionDynamic)
	{
		XMFLOAT4 const bitmapPos = CalculateFinalPosition();
		UpdateBuffers(bitmapPos);
	}

	m_UIElement->Update(delta);
}

void UIBitmap::Draw()
{
	if (bShouldDraw)
	{
		auto context = ApplicationNew::Get().GetContext();

		context->PSSetShaderResources(0, 1, &m_Texture);

		m_UIElement->Draw();
	}
}

void UIBitmap::UpdateBuffers(XMFLOAT4 const& inVertsPos)
{
	auto context = ApplicationNew::Get().GetContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	SimpleQuad verts[] =
	{
		{ XMFLOAT3(inVertsPos.x, inVertsPos.w, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(inVertsPos.x, inVertsPos.z, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(inVertsPos.y, inVertsPos.z, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(inVertsPos.y, inVertsPos.w, 0.0f),XMFLOAT2(1.0f, 1.0f) }
	};

	HRESULT hr = context->Map(m_UIElement->GetAppearance()->GetObjectMesh().VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return;
	}

	auto* vertsPtr = reinterpret_cast<SimpleQuad*>(mappedResource.pData);

	memcpy(vertsPtr, verts, sizeof(SimpleQuad) * 4);

	context->Unmap(m_UIElement->GetAppearance()->GetObjectMesh().VertexBuffer.Get(), 0);
}

XMFLOAT4 UIBitmap::CalculateFinalPosition()
{
	auto aspectRatio = m_ScreenSize.x / m_ScreenSize.y;

	float left, right, top, bottom;

	XMFLOAT2 pos = bIsPositionDynamic ? m_DynamicPos : m_BitmapPosition;

	left = ((m_ScreenSize.x / 2.0f) * -1.0f) + pos.x;
	right = left + m_BitmapSize.x;
	top = (m_ScreenSize.y / 2.0f) - pos.y;
	bottom = top - m_BitmapSize.y;

	switch (m_Origin)
	{
	case UIOriginPoint::TopLeft:
	{
		// UI bitmaps are top-left default
	}
	break;
	case UIOriginPoint::TopMiddle:
	{
		// Offset element to the left by half the bitmap width
		float const bitmapHalfWidth = (m_BitmapSize.x / 2.f);
		left -= bitmapHalfWidth;
		right -= bitmapHalfWidth;
	}
	break;
	case UIOriginPoint::TopRight:
	{
		// Offset element to the left by full bitmap width
		left -= m_BitmapSize.x;
		right -= m_BitmapSize.x;
	}
	break;
	case UIOriginPoint::CentreLeft:
	{
		// Offset element up by half bitmap height
		float const bitmapHalfHeight = (m_BitmapSize.y / 2.f);
		top += bitmapHalfHeight;
		bottom += bitmapHalfHeight;
	}
	break;
	case UIOriginPoint::Centre:
	{
		float const bitmapHalfHeight = (m_BitmapSize.y / 2.f);
		float const bitmapHalfWidth = (m_BitmapSize.x / 2.f);
		// Offset element up and to the left by half bitmap width + half bitmap height
		left -= bitmapHalfWidth;
		right -= bitmapHalfWidth;
		top += bitmapHalfHeight;
		bottom += bitmapHalfHeight;
	}
	break;
	case UIOriginPoint::CentreRight:
	{
		float const bitmapHalfHeight = (m_BitmapSize.y / 2.f);
		// Offset element up and to the left by full bitmap width + half bitmap height
		left -= m_BitmapSize.x;
		right -= m_BitmapSize.x;
		top += bitmapHalfHeight;
		bottom += bitmapHalfHeight;
	}
	break;
	case UIOriginPoint::BottomLeft:
	{
		// Offset element up by full bitmap height
		top += m_BitmapSize.y;
		bottom += m_BitmapSize.y;
	}
	break;
	case UIOriginPoint::BottomCentre:
	{
		float const bitmapHalfWidth = (m_BitmapSize.x / 2.f);
		// Offset element up and to the left by half bitmap width + full bitmap height
		left -= bitmapHalfWidth;
		right -= bitmapHalfWidth;
		top += m_BitmapSize.y;
		bottom += m_BitmapSize.y;
	}
	break;
	case UIOriginPoint::BottomRight:
	{
		// Offset element up and to the left by full bitmap width + full bitmap height
		left -= m_BitmapSize.x;
		right -= m_BitmapSize.x;
		top += m_BitmapSize.y;
		bottom += m_BitmapSize.y;
	}
	break;
	default:
		break;
	}

	left += m_BitmapOffset.x;
	right += m_BitmapOffset.x;
	top += m_BitmapOffset.y;
	bottom += m_BitmapOffset.y;

	/*if (bIsPositionDynamic)
	{
		left += m_DynamicPos.x;
		right += m_DynamicPos.x;
		top += m_DynamicPos.y;
		bottom += m_DynamicPos.y;
	}*/

	return XMFLOAT4(left, right, top, bottom);
}
