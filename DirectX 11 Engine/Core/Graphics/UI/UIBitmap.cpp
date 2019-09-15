#include "UIBitmap.h"

UIBitmap::UIBitmap(D3DClass* d3dClass, BufferClass* bufferClass) : _d3dClass(d3dClass), _bufferClass(bufferClass)
{
}

UIBitmap::~UIBitmap()
{
}

void UIBitmap::Initialise(XMFLOAT2 screenSize, XMFLOAT2 bitmapSize, ID3D11ShaderResourceView* texture)
{
	_screenSize = screenSize;
	_bitmapSize = bitmapSize;
	_texture = texture;
	_bitmapPrevPosition = XMFLOAT2(-1, -1);

	_bufferClass->CreateQuadDynamic(&_vertexBuffer, &_indexBuffer);

	_uiQuad.vertexBuffer = _vertexBuffer;
	_uiQuad.indexBuffer = _indexBuffer;
	_uiQuad.numberOfIndices = 6;
	_uiQuad.vertexBufferOffset = 0;
	_uiQuad.vertexBufferStride = sizeof(SimpleQuad);

	auto* uiTransform = new Transform();
	uiTransform->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	uiTransform->SetScale(XMFLOAT3(1, 1, 1.0f));
	uiTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto* uiAppearance = new Appearance(_uiQuad, ObjectMaterial{});

	_uiElement = new SceneElement("UI Square", uiTransform, uiAppearance);
}

void UIBitmap::UpdateScreenSize(XMFLOAT2 newScreenSize)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr;

	if (newScreenSize.x == _screenSize.x && newScreenSize.y == _screenSize.y)
	{
		return;
	}

	_screenSize = newScreenSize;

	auto aspectRatio = _screenSize.x / _screenSize.y;

	float left = (_screenSize.x / 2) * -1 + _bitmapPrevPosition.x;
	float right = left + _bitmapSize.x;
	float top = (_screenSize.y / 2) - _bitmapPrevPosition.y;
	float bottom = top - _bitmapSize.y;

	SimpleQuad verts[] =
	{
		{ XMFLOAT3(left, bottom, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(left, top, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(right, top, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(right, bottom, 0.0f),XMFLOAT2(1.0f, 1.0f) }
	};

	_d3dClass->GetContext()->Map(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	auto* vertsPtr = reinterpret_cast<SimpleQuad*>(mappedResource.pData);

	memcpy(vertsPtr, verts, sizeof(SimpleQuad) * 4);

	_d3dClass->GetContext()->Unmap(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer, 0);
}

void UIBitmap::MoveBitmap(XMFLOAT2 newPos)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr;

	if(newPos.x == _bitmapPrevPosition.x && newPos.y == _bitmapPrevPosition.y)
	{
		return;
	}

	_bitmapPrevPosition = newPos;

	auto aspectRatio = _screenSize.x / _screenSize.y;

	float left = (_screenSize.x / 2) * -1 + newPos.x;
	float right = left + _bitmapSize.x;
	float top = (_screenSize.y / 2) - newPos.y;
	float bottom = top - _bitmapSize.y;

	SimpleQuad verts[] =
	{
		{ XMFLOAT3(left, bottom, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(left, top, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(right, top, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(right, bottom, 0.0f),XMFLOAT2(1.0f, 1.0f) }
	};

	_d3dClass->GetContext()->Map(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	auto* vertsPtr = reinterpret_cast<SimpleQuad*>(mappedResource.pData);

	memcpy(vertsPtr, verts, sizeof(SimpleQuad) * 4);

	_d3dClass->GetContext()->Unmap(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer, 0);
}

void UIBitmap::Update(float delta)
{
	MoveBitmap(_bitmapPrevPosition);
	_uiElement->Update(delta);
}

void UIBitmap::Draw()
{
	_d3dClass->GetContext()->PSSetShaderResources(0, 1, &_texture);

	_uiElement->Draw(_d3dClass->GetContext());
}
