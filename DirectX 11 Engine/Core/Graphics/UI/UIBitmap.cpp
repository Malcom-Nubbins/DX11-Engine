#include "UIBitmap.h"

#include "../../ApplicationNew.h"

UIBitmap::UIBitmap()
{
}

UIBitmap::~UIBitmap()
{
}

void UIBitmap::Cleanup()
{
	_texture->Release();
	_vertexBuffer->Release();
	_indexBuffer->Release();

	_uiElement->Cleanup();
}

void UIBitmap::Initialise(XMFLOAT2 const screenSize, XMFLOAT2 const bitmapSize, 
						UIOriginPoint const originPoint,
						ID3D11ShaderResourceView* const texture)
{
	_screenSize = screenSize;
	_bitmapSize = bitmapSize;
	m_Origin = originPoint;
	_texture = texture;
	_bitmapPrevPosition = XMFLOAT2(-1, -1);

	BufferClass::CreateQuadDynamic(&_vertexBuffer, &_indexBuffer);

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	char const uiQuadVBName[] = "UI Quad VB";
	char const uiQuadIBName[] = "UI Quad IB";

	_vertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(uiQuadVBName) - 1, uiQuadVBName);
	_indexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(uiQuadIBName) - 1, uiQuadIBName);
#endif

	_uiQuad.vertexBuffer = _vertexBuffer;
	_uiQuad.vertexBuffer->AddRef();
	_uiQuad.indexBuffer = _indexBuffer;
	_uiQuad.indexBuffer->AddRef();
	_uiQuad.numberOfIndices = 6;
	_uiQuad.vertexBufferOffset = 0;
	_uiQuad.vertexBufferStride = sizeof(SimpleQuad);

	auto uiTransform = std::make_shared<Transform>();
	uiTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	uiTransform->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	uiTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto uiAppearance = std::make_shared<Appearance>(_uiQuad, ObjectMaterial{});

	_uiElement = new SceneElement("UI Square", *uiTransform, *uiAppearance);
}

void UIBitmap::UpdateScreenSize(XMFLOAT2 newScreenSize)
{
	if (newScreenSize.x == _screenSize.x && newScreenSize.y == _screenSize.y)
	{
		return;
	}

	_screenSize = newScreenSize;

	XMFLOAT4 const bitmapPos = CalculatePosition();

	UpdateBuffers(bitmapPos);
}

void UIBitmap::MoveBitmap(XMFLOAT2 newPos)
{
	if(newPos.x == _bitmapPrevPosition.x && newPos.y == _bitmapPrevPosition.y)
	{
		return;
	}

	_bitmapPrevPosition = newPos;

	XMFLOAT4 const bitmapPos = CalculatePosition();

	UpdateBuffers(bitmapPos);
}

void UIBitmap::Update(double delta)
{
	MoveBitmap(_bitmapPrevPosition);
	_uiElement->Update(delta);
}

void UIBitmap::Draw()
{
	auto context = ApplicationNew::Get().GetContext();

	context->PSSetShaderResources(0, 1, &_texture);

	_uiElement->Draw();
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

XMFLOAT4 UIBitmap::CalculatePosition()
{
	auto aspectRatio = _screenSize.x / _screenSize.y;

	float left, right, top, bottom;

	left = ((_screenSize.x / 2.0f) * -1.0f) + _bitmapPrevPosition.x;
	right = left + _bitmapSize.x;
	top = (_screenSize.y / 2.0f) - _bitmapPrevPosition.y;
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

	return XMFLOAT4(left, right, top, bottom);
}
