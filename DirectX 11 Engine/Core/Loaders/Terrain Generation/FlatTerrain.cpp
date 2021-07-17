#include "FlatTerrain.h"
#include "../../ApplicationNew.h"

FlatTerrain::FlatTerrain()
{
	_gridVertices = std::vector<SimpleVertex>();
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
	_gridMesh = nullptr;
}


FlatTerrain::~FlatTerrain()
{
}

void FlatTerrain::Cleanup()
{
	if (_vertexBuffer) _vertexBuffer->Release();
	if (_indexBuffer) _indexBuffer->Release();
	
	delete _gridMesh;
	_gridMesh = nullptr;

	_terrainGO->Cleanup();
	delete _terrainGO;
	_terrainGO = nullptr;
}

void FlatTerrain::SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain)
{
	_info.gridWidth = terrainWidth;
	_info.gridDepth = terrainDepth;
	_info.heightMapWidth = sizeOfTerrain;
	_info.heightMapHeight = sizeOfTerrain;
}

void FlatTerrain::RegenerateTerrain()
{
	// Do nothing
}

void FlatTerrain::GenerateTerrain()
{
	_info.cellSpacing = _info.gridWidth / _info.heightMapHeight;

	UINT vertexCount = _info.heightMapWidth * _info.heightMapHeight;
	UINT faceCount = (_info.heightMapWidth - 1)*(_info.heightMapHeight - 1) * 2;

	float halfWidth = 0.5f * _info.gridWidth;
	float halfDepth = 0.5f * _info.gridDepth;

	float dx = _info.gridWidth / (_info.heightMapWidth - 1);
	float dz = _info.gridDepth / (_info.heightMapHeight - 1);

	float du = 10.0f / (_info.heightMapWidth - 1);
	float dv = 10.0f / (_info.heightMapHeight - 1);

	_gridMesh = new GridMeshData();
	_gridMesh->Vertices.resize(vertexCount);

	for (UINT i = 0; i < _info.heightMapWidth; ++i)
	{
		float z = halfDepth - i * dz;

		for (UINT j = 0; j < _info.heightMapHeight; ++j)
		{
			float x = -halfWidth + j * dx;

			_gridMesh->Vertices[i*_info.heightMapHeight + j].position = XMFLOAT3(x, 0.0f, z);
			_gridMesh->Vertices[i*_info.heightMapHeight + j].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			_gridMesh->Vertices[i*_info.heightMapHeight + j].texCoord.x = j * du;
			_gridMesh->Vertices[i*_info.heightMapHeight + j].texCoord.y = i * dv;

			_gridMesh->Vertices[i*_info.heightMapHeight + j].tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
			_gridMesh->Vertices[i*_info.heightMapHeight + j].binormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
	}



	for (UINT i = 0; i < _gridMesh->Vertices.size(); ++i)
	{
		_heights.push_back(_gridMesh->Vertices[i].position.y);
	}

	_gridMesh->Indices.resize(faceCount * 3);

	UINT k = 0;
	for (UINT i = 0; i < _info.heightMapWidth - 1; ++i)
	{
		for (UINT j = 0; j < _info.heightMapHeight - 1; ++j)
		{
			_gridMesh->Indices[k] = i * _info.heightMapHeight + j;
			_gridMesh->Indices[k + 1] = i * _info.heightMapHeight + j + 1;
			_gridMesh->Indices[k + 2] = (i + 1)*_info.heightMapHeight + j;
			_gridMesh->Indices[k + 3] = (i + 1)*_info.heightMapHeight + j;
			_gridMesh->Indices[k + 4] = i * _info.heightMapHeight + j + 1;
			_gridMesh->Indices[k + 5] = (i + 1)*_info.heightMapHeight + j + 1;
			k += 6;
		}
	}

	GetNormals(faceCount, vertexCount);
	CreateBuffers();
}

HRESULT FlatTerrain::CreateBuffers()
{
	HRESULT hr;
	// Vertex Buffer
	_gridVertices.resize(_gridMesh->Vertices.size());
	UINT k = 0;

	for (size_t i = 0; i < _gridMesh->Vertices.size(); i++, k++)
	{
		_gridVertices[k].pos = _gridMesh->Vertices[i].position;
		_gridVertices[k].normal = _gridMesh->Vertices[i].normal;
		_gridVertices[k].texCoord = _gridMesh->Vertices[i].texCoord;
		_gridVertices[k].tangent = _gridMesh->Vertices[i].tangent;
		_gridVertices[k].binormal = _gridMesh->Vertices[i].binormal;
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(SimpleVertex) * static_cast<UINT>(_gridMesh->Vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGridData;
	ZeroMemory(&InitGridData, sizeof(InitGridData));
	InitGridData.pSysMem = &_gridVertices[0];

	hr = ApplicationNew::Get().GetDevice()->CreateBuffer(&vbd, &InitGridData, &_vertexBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	
	// Index Buffer
	_gridIndices.insert(_gridIndices.end(), _gridMesh->Indices.begin(), _gridMesh->Indices.end());

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(LONG) * static_cast<UINT>(_gridMesh->Indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGridIndexData;
	ZeroMemory(&InitGridIndexData, sizeof(InitGridIndexData));
	InitGridIndexData.pSysMem = &_gridIndices[0];

	hr = ApplicationNew::Get().GetDevice()->CreateBuffer(&ibd, &InitGridIndexData, &_indexBuffer);
	if (FAILED(hr))
		return hr;

#if defined(_DEBUG) && (USE_D3D11_DEBUGGING == 1)
	char const flatTerrainVBName[] = "FlatTerrainVB";
	_vertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(flatTerrainVBName) - 1, flatTerrainVBName);

	char const flatTerrainIBName[] = "FlatTerrainIB";
	_indexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(flatTerrainIBName) - 1, flatTerrainIBName);
#endif

	

	return S_OK;
}

void FlatTerrain::GetNormals(UINT faceCount, UINT vertexCount)
{
	for (UINT i = 0; i < faceCount; ++i)
	{
		UINT i0 = _gridMesh->Indices[i * 3 + 0];
		UINT i1 = _gridMesh->Indices[i * 3 + 1];
		UINT i2 = _gridMesh->Indices[i * 3 + 2];

		TempVertex v1, v2, v3;
		v1.pos = _gridMesh->Vertices[i0].position;
		v2.pos = _gridMesh->Vertices[i1].position;
		v3.pos = _gridMesh->Vertices[i2].position;

		XMFLOAT3 e0Float = MathsHandler::Subtract(v2.pos, v1.pos);
		XMFLOAT3 e1Float = MathsHandler::Subtract(v3.pos, v1.pos);
		XMVECTOR e0 = XMLoadFloat3(&e0Float);
		XMVECTOR e1 = XMLoadFloat3(&e1Float);
		XMVECTOR normal = XMVector3Cross(e0, e1);
		XMFLOAT3 normalFloat;
		XMStoreFloat3(&normalFloat, normal);

		_gridMesh->Vertices[i0].normal = MathsHandler::Add(_gridMesh->Vertices[i0].normal, normalFloat);
		_gridMesh->Vertices[i1].normal = MathsHandler::Add(_gridMesh->Vertices[i1].normal, normalFloat);
		_gridMesh->Vertices[i2].normal = MathsHandler::Add(_gridMesh->Vertices[i2].normal, normalFloat);

		v1.pos = _gridMesh->Vertices[i0].position;
		v1.normal = _gridMesh->Vertices[i0].normal;
		v1.tex = _gridMesh->Vertices[i0].texCoord;

		v2.pos = _gridMesh->Vertices[i1].position;
		v2.normal = _gridMesh->Vertices[i1].normal;
		v2.tex = _gridMesh->Vertices[i1].texCoord;

		v3.pos = _gridMesh->Vertices[i2].position;
		v3.normal = _gridMesh->Vertices[i2].normal;
		v3.tex = _gridMesh->Vertices[i2].texCoord;

		_gridMesh->Vertices[i0].tangent = MathsHandler::CalculateTangent(v1, v2, v3);
		_gridMesh->Vertices[i1].tangent = MathsHandler::CalculateTangent(v1, v2, v3);
		_gridMesh->Vertices[i2].tangent = MathsHandler::CalculateTangent(v1, v2, v3);

		_gridMesh->Vertices[i0].binormal = MathsHandler::CalculateBinormal(v1, v2, v3);
		_gridMesh->Vertices[i1].binormal = MathsHandler::CalculateBinormal(v1, v2, v3);
		_gridMesh->Vertices[i2].binormal = MathsHandler::CalculateBinormal(v1, v2, v3);

	}

	for (UINT i = 0; i < vertexCount; ++i)
	{
		XMVECTOR normal = XMLoadFloat3(&_gridMesh->Vertices[i].normal);
		XMVector3Normalize(normal);
		XMStoreFloat3(&_gridMesh->Vertices[i].normal, normal);
	}
}

void FlatTerrain::Update(float deltaTime)
{
	_terrainGO->Update(deltaTime);
}

void FlatTerrain::DepthNormalDraw(MatrixBuffer mb, ID3D11Buffer * constBuffer)
{
	mb.World = XMMatrixTranspose(XMLoadFloat4x4(&_terrainGO->GetTransform()->GetWorld()));
	ApplicationNew::Get().GetContext()->UpdateSubresource(constBuffer, 0, nullptr, &mb, 0, 0);
	_terrainGO->Draw();
}

void FlatTerrain::ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer * constBuffer)
{
	mb.World = XMMatrixTranspose(XMLoadFloat4x4(&_terrainGO->GetTransform()->GetWorld()));
	ApplicationNew::Get().GetContext()->UpdateSubresource(constBuffer, 0, nullptr, &mb, 0, 0);
	_terrainGO->Draw();
}

void FlatTerrain::Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer * matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView * texArray[])
{
	auto context = ApplicationNew::Get().GetContext();

	auto* transform = _terrainGO->GetTransform();
	auto* appearance = _terrainGO->GetAppearance();

	auto material = appearance->GetObjectMaterial();

	// Copy material to shader
	cb.surface.ambient = material.ambient;
	cb.surface.diffuse = material.diffuse;
	cb.surface.specular = material.specular;

	// Set world matrix
	mb.World = XMMatrixTranspose(XMLoadFloat4x4(&transform->GetWorld()));

	ID3D11ShaderResourceView* texture;
	// Set texture
	if (appearance->HasColourTexture())
	{
		texture = appearance->GetColourTex();
		context->PSSetShaderResources(0, 1, &texture);
		cb.useColourTex = 1.0f;
	}
	else
	{
		cb.useColourTex = 0.0f;
	}

	if (appearance->HasNormalMap())
	{
		cb.useBumpMap = 1.0f;
		texture = appearance->GetNormalMap();
		context->PSSetShaderResources(2, 1, &texture);
	}
	else
	{
		cb.useBumpMap = 0.0f;
	}

	if (_terrainGO->IsAffectedByLight())
	{
		cb.affectedByLight = 1.0f;
	}
	else
	{
		cb.affectedByLight = 0.0f;
	}

	// Update constant buffer
	context->UpdateSubresource(matrixBuffer, 0, nullptr, &mb, 0, 0);
	context->UpdateSubresource(objValuesBuffer, 0, nullptr, &cb, 0, 0);
	_terrainGO->Draw();
}

float FlatTerrain::GetWidth()
{
	return _info.gridWidth * _terrainGO->GetTransform()->GetScale().x;
}

float FlatTerrain::GetDepth()
{
	return _info.gridDepth * _terrainGO->GetTransform()->GetScale().z;
}

float FlatTerrain::GetHeight(float camX, float camZ)
{
	return _terrainGO->GetTransform()->GetPosition().y;
}
