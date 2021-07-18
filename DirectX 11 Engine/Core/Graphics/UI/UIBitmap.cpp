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

void UIBitmap::Initialise(XMFLOAT2 screenSize, XMFLOAT2 bitmapSize, ID3D11ShaderResourceView* texture)
{
	_screenSize = screenSize;
	_bitmapSize = bitmapSize;
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
	uiTransform->SetScale(XMFLOAT3(1, 1, 1.0f));
	uiTransform->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	auto uiAppearance = std::make_shared<Appearance>(_uiQuad, ObjectMaterial{});

	_uiElement = new SceneElement("UI Square", *uiTransform, *uiAppearance);
}

void UIBitmap::UpdateScreenSize(XMFLOAT2 newScreenSize)
{
	auto context = ApplicationNew::Get().GetContext();

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

	hr = context->Map(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return;
	}

	auto* vertsPtr = reinterpret_cast<SimpleQuad*>(mappedResource.pData);

	memcpy(vertsPtr, verts, sizeof(SimpleQuad) * 4);

	context->Unmap(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer.Get(), 0);
}

void UIBitmap::MoveBitmap(XMFLOAT2 newPos)
{
	auto context = ApplicationNew::Get().GetContext();

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

	hr = context->Map(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return;
	}

	auto* vertsPtr = reinterpret_cast<SimpleQuad*>(mappedResource.pData);

	memcpy(vertsPtr, verts, sizeof(SimpleQuad) * 4);

	context->Unmap(_uiElement->GetAppearance()->GetObjectMesh().vertexBuffer.Get(), 0);
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
