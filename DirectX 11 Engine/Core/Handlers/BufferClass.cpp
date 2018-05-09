#include "BufferClass.h"

BufferClass::BufferClass(D3DClass* d3dClass) : _d3dClass(d3dClass)
{
}


BufferClass::~BufferClass()
{
}

HRESULT BufferClass::CreateGroundPlane(ID3D11Buffer ** vertexBuffer, ID3D11Buffer ** indexBuffer)
{
	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;

	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 5.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(5.0f, 5.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(5.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
	};

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _d3dClass->GetDevice()->CreateBuffer(&bd, &InitData, vertexBuffer);

	if (FAILED(hr))
		return hr;

	unsigned long planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};


	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned long) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _d3dClass->GetDevice()->CreateBuffer(&bd, &InitData, indexBuffer);

	if (FAILED(hr))
		return hr;
	return S_OK;
}

HRESULT BufferClass::CreateQuad(ID3D11Buffer ** vertexBuffer, ID3D11Buffer ** indexBuffer)
{
	HRESULT hr;
	SimpleQuad vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f),XMFLOAT2(1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleQuad) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	hr = _d3dClass->GetDevice()->CreateBuffer(&bd, &InitData, vertexBuffer);

	if (FAILED(hr))
		return hr;

	// Create index buffer
	unsigned long indices[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned long) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = indices;
	hr = _d3dClass->GetDevice()->CreateBuffer(&bd, &InitData, indexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void BufferClass::SetIABuffers(ObjectMesh mesh)
{

}

void BufferClass::SetVertexShaderBuffers(ID3D11Buffer** buffer)
{
	SetVertexShaderBuffers(buffer, 0);
}

void BufferClass::SetVertexShaderBuffers(ID3D11Buffer** buffer, int index)
{
	_d3dClass->GetContext()->VSSetConstantBuffers(index, 1, buffer);
}

void BufferClass::SetPixelShaderBuffers(ID3D11Buffer** buffer)
{
	SetPixelShaderBuffers(buffer, 0);
}

void BufferClass::SetPixelShaderBuffers(ID3D11Buffer** buffer, int index)
{
	_d3dClass->GetContext()->PSSetConstantBuffers(index, 1, buffer);
}


