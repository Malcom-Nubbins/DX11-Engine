#include "UIBitmap.h"

#include "../../ApplicationNew.h"
#include "../../DX11Engine.h"

UIBitmap::UIBitmap()
	: m_ShouldDraw(true)
	, m_IsPositionDynamic(false)
	, m_DynamicPos(0.0f, 0.0f)
{
}

UIBitmap::~UIBitmap()
{
}

void UIBitmap::Cleanup()
{
	_texture->Release();
	
	_uiElement->Cleanup();
}

void UIBitmap::Initialise(XMFLOAT2 const screenSize, S_UIElementInfo const inElementInfo)
{
	auto textureHandler = DX11Engine::Get().GetTextureHandler();

	_screenSize = screenSize;
	_bitmapSize = inElementInfo.m_Size;
	m_Anchor = inElementInfo.m_AnchorPoint;
	m_Origin = inElementInfo.m_OriginPoint;
	m_ElementName = inElementInfo.m_ElementName;
	m_Offset = inElementInfo.m_Offset;
	_texture = textureHandler->GetTextureByName(inElementInfo.m_TextureName.c_str());

	BufferClass::CreateQuadDynamic(&_uiQuad.vertexBuffer, &_uiQuad.indexBuffer);

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	char const uiQuadVBName[] = "UI Quad VB";
	char const uiQuadIBName[] = "UI Quad IB";

	_uiQuad.vertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(uiQuadVBName) - 1, uiQuadVBName);
	_uiQuad.indexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(uiQuadIBName) - 1, uiQuadIBName);
#endif

	_uiQuad.numberOfIndices = 6;
	_uiQuad.vertexBufferOffset = 0;
	_uiQuad.vertexBufferStride = sizeof(SimpleQuad);

	auto uiTransform = std::make_shared<Transform>();
	uiTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	uiTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	uiTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto uiAppearance = std::make_shared<Appearance>(_uiQuad, ObjectMaterial{});

	_uiElement = new SceneElement(GetStringHash(m_ElementName), *uiTransform, *uiAppearance);

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
		m_Position = XMFLOAT2(0.0f, 0.0f);
	}
	break;
	case UIAnchorPoint::TopMiddle:
	{
		m_Position = XMFLOAT2(_screenSize.x / 2.0f, 0.0f);
	}
	break;
	case UIAnchorPoint::TopRight:
	{
		m_Position = XMFLOAT2(_screenSize.x, 0.0f);
	}
	break;
	case UIAnchorPoint::CentreLeft:
	{
		m_Position = XMFLOAT2(0.0f, _screenSize.y / 2.0f);
	}
	break;
	case UIAnchorPoint::Centre:
	{
		m_Position = XMFLOAT2(_screenSize.x / 2.0f, _screenSize.y / 2.0f);
	}
	break;
	case UIAnchorPoint::CentreRight:
	{
		m_Position = XMFLOAT2(_screenSize.x, _screenSize.y / 2.0f);
	}
	break;
	case UIAnchorPoint::BottomLeft:
	{
		m_Position = XMFLOAT2(0.0f, _screenSize.y);
	}
	break;
	case UIAnchorPoint::BottomCentre:
	{
		m_Position = XMFLOAT2(_screenSize.x / 2.0f, _screenSize.y);
	}
	break;
	case UIAnchorPoint::BottomRight:
	{
		m_Position = XMFLOAT2(_screenSize.x, _screenSize.y);
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
	if (newScreenSize.x == _screenSize.x && newScreenSize.y == _screenSize.y)
	{
		return;
	}

	_screenSize = newScreenSize;

	SetPosition();

	XMFLOAT4 const bitmapPos = CalculateFinalPosition();

	UpdateBuffers(bitmapPos);
}

void UIBitmap::Update(double delta)
{
	if (m_IsPositionDynamic)
	{
		XMFLOAT4 const bitmapPos = CalculateFinalPosition();
		UpdateBuffers(bitmapPos);
	}

	_uiElement->Update(delta);
}

void UIBitmap::Draw()
{
	if (m_ShouldDraw)
	{
		auto context = ApplicationNew::Get().GetContext();

		context->PSSetShaderResources(0, 1, &_texture);

		_uiElement->Draw();
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

	HRESULT hr = context->Map(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return;
	}

	auto* vertsPtr = reinterpret_cast<SimpleQuad*>(mappedResource.pData);

	memcpy(vertsPtr, verts, sizeof(SimpleQuad) * 4);

	context->Unmap(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer.Get(), 0);
}

XMFLOAT4 UIBitmap::CalculateFinalPosition()
{
	auto aspectRatio = _screenSize.x / _screenSize.y;

	float left, right, top, bottom;

	XMFLOAT2 pos = m_IsPositionDynamic ? m_DynamicPos : m_Position;

	left = ((_screenSize.x / 2.0f) * -1.0f) + pos.x;
	right = left + _bitmapSize.x;
	top = (_screenSize.y / 2.0f) - pos.y;
	bottom = top - _bitmapSize.y;

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
		float const bitmapHalfWidth = (_bitmapSize.x / 2.f);
		left -= bitmapHalfWidth;
		right -= bitmapHalfWidth;
	}
	break;
	case UIOriginPoint::TopRight:
	{
		// Offset element to the left by full bitmap width
		left -= _bitmapSize.x;
		right -= _bitmapSize.x;
	}
	break;
	case UIOriginPoint::CentreLeft:
	{
		// Offset element up by half bitmap height
		float const bitmapHalfHeight = (_bitmapSize.y / 2.f);
		top += bitmapHalfHeight;
		bottom += bitmapHalfHeight;
	}
	break;
	case UIOriginPoint::Centre:
	{
		float const bitmapHalfHeight = (_bitmapSize.y / 2.f);
		float const bitmapHalfWidth = (_bitmapSize.x / 2.f);
		// Offset element up and to the left by half bitmap width + half bitmap height
		left -= bitmapHalfWidth;
		right -= bitmapHalfWidth;
		top += bitmapHalfHeight;
		bottom += bitmapHalfHeight;
	}
	break;
	case UIOriginPoint::CentreRight:
	{
		float const bitmapHalfHeight = (_bitmapSize.y / 2.f);
		// Offset element up and to the left by full bitmap width + half bitmap height
		left -= _bitmapSize.x;
		right -= _bitmapSize.x;
		top += bitmapHalfHeight;
		bottom += bitmapHalfHeight;
	}
	break;
	case UIOriginPoint::BottomLeft:
	{
		// Offset element up by full bitmap height
		top += _bitmapSize.y;
		bottom += _bitmapSize.y;
	}
	break;
	case UIOriginPoint::BottomCentre:
	{
		float const bitmapHalfWidth = (_bitmapSize.x / 2.f);
		// Offset element up and to the left by half bitmap width + full bitmap height
		left -= bitmapHalfWidth;
		right -= bitmapHalfWidth;
		top += _bitmapSize.y;
		bottom += _bitmapSize.y;
	}
	break;
	case UIOriginPoint::BottomRight:
	{
		// Offset element up and to the left by full bitmap width + full bitmap height
		left -= _bitmapSize.x;
		right -= _bitmapSize.x;
		top += _bitmapSize.y;
		bottom += _bitmapSize.y;
	}
	break;
	default:
		break;
	}

	left += m_Offset.x;
	right += m_Offset.x;
	top += m_Offset.y;
	bottom += m_Offset.y;

	/*if (m_IsPositionDynamic)
	{
		left += m_DynamicPos.x;
		right += m_DynamicPos.x;
		top += m_DynamicPos.y;
		bottom += m_DynamicPos.y;
	}*/

	return XMFLOAT4(left, right, top, bottom);
}
