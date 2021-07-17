#include "DiamondSquareTerrain.h"
#include "../../ApplicationNew.h"

DiamondSquareTerrain::DiamondSquareTerrain()
{
	_gridVertices = std::vector<SimpleVertex>();
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
	_gridMesh = nullptr;
	_terrainGO = nullptr;
	_finishedBuilding = false;
}

DiamondSquareTerrain::~DiamondSquareTerrain()
{
}

void DiamondSquareTerrain::Cleanup()
{
	if (_vertexBuffer) _vertexBuffer->Release();
	if (_indexBuffer) _indexBuffer->Release();
	delete _gridMesh;
	_gridMesh = nullptr;

	_terrainGO->Cleanup();
	delete _terrainGO;
	_terrainGO = nullptr;

	delete[] _heightMap;
}

void DiamondSquareTerrain::GenerateHeightmap(int sampleSize, double scale)
{
	int halfStep = sampleSize / 2;

	for (unsigned int y = halfStep; y < _info.heightMapHeight + halfStep; y += sampleSize)
	{
		for (unsigned int x = halfStep; x < _info.heightMapWidth + halfStep; x += sampleSize)
		{
			SampleSquare(x, y, sampleSize, MathsHandler::RandomFloat(-1.0f, 1.0f) * scale);
		}
	}

	for (unsigned int y = 0; y < _info.heightMapHeight; y += sampleSize)
	{
		for (unsigned int x = 0; x < _info.heightMapWidth; x += sampleSize)
		{
			SampleDiamond(x + halfStep, y, sampleSize, MathsHandler::RandomFloat(-1.0f, 1.0f) * scale);
			SampleDiamond(x, y + halfStep, sampleSize, MathsHandler::RandomFloat(-1.0f, 1.0f) * scale);
		}
	}
}

void DiamondSquareTerrain::SampleSquare(int x, int y, int size, double value)
{
	int hs = size / 2;

	double a = Sample(x - hs, y - hs);
	double b = Sample(x + hs, y - hs);
	double c = Sample(x - hs, y + hs);
	double d = Sample(x + hs, y + hs);
	SetSample(x, y, ((a + b + c + d) / 4.0) + value);
}

void DiamondSquareTerrain::SampleDiamond(int x, int y, int size, double value)
{
	int hs = size / 2;

	double a = Sample(x - hs, y);
	double b = Sample(x + hs, y);
	double c = Sample(x, y - hs);
	double d = Sample(x, y + hs);
	SetSample(x, y, ((a + b + c + d) / 4.0) + value);
}

double DiamondSquareTerrain::Sample(int x, int y)
{
	return _heightMap[((x & (_info.heightMapWidth - 1)) + (y & (_info.heightMapHeight - 1)) * _info.heightMapWidth)];
}

void DiamondSquareTerrain::SetSample(int x, int y, double value)
{
	if (value >= 0.0f)
		_heightMap[(x & (_info.heightMapWidth - 1)) + (y & (_info.heightMapHeight - 1)) * _info.heightMapWidth] = value;
	else
		_heightMap[(x & (_info.heightMapWidth - 1)) + (y & (_info.heightMapHeight - 1)) * _info.heightMapWidth] = 0.0f;
}

void DiamondSquareTerrain::SmoothTerrain(float smoothnessAmount)
{
	unsigned int x, z;

	for (x = 1; x < _info.heightMapWidth; ++x)
	{
		for (z = 0; z < _info.heightMapHeight; ++z)
		{
			_gridMesh->Vertices[x * _info.heightMapHeight + z].position.y =
				_gridMesh->Vertices[(x - 1)*_info.heightMapHeight + z].position.y * (1 - smoothnessAmount) +
				_gridMesh->Vertices[x*_info.heightMapHeight + z].position.y * smoothnessAmount;
		}
	}

	for (x = _info.heightMapWidth - 2; x < 0; --x)
	{
		for (z = 0; z < _info.heightMapHeight; ++z)
		{
			_gridMesh->Vertices[x * _info.heightMapHeight + z].position.y =
				_gridMesh->Vertices[(x + 1)*_info.heightMapHeight + z].position.y * (1 - smoothnessAmount) +
				_gridMesh->Vertices[x*_info.heightMapHeight + z].position.y * smoothnessAmount;
		}
	}

	for (x = 0; x < _info.heightMapWidth; ++x)
	{
		for (z = 1; z < _info.heightMapHeight; ++z)
		{
			_gridMesh->Vertices[x * _info.heightMapHeight + z].position.y =
				_gridMesh->Vertices[x*_info.heightMapHeight + (z - 1)].position.y * (1 - smoothnessAmount) +
				_gridMesh->Vertices[x*_info.heightMapHeight + z].position.y * smoothnessAmount;
		}
	}

	for (x = 0; x < _info.heightMapWidth; ++x)
	{
		for (z = _info.heightMapHeight; z < 0; --z)
		{
			_gridMesh->Vertices[x * _info.heightMapHeight + z].position.y =
				_gridMesh->Vertices[x*_info.heightMapHeight + (z + 1)].position.y * (1 - smoothnessAmount) +
				_gridMesh->Vertices[(x + 1)*_info.heightMapHeight + z].position.y * smoothnessAmount;
		}
	}
}

void DiamondSquareTerrain::SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain)
{
	_info.gridWidth = terrainWidth;
	_info.gridDepth = terrainDepth;
	_info.heightMapWidth = sizeOfTerrain;
	_info.heightMapHeight = sizeOfTerrain;
}

void DiamondSquareTerrain::RegenerateTerrain()
{
	_vertexBuffer->Release();
	_indexBuffer->Release();
	_gridVertices.clear();
	_gridIndices.clear();
	delete _gridMesh;
	delete[] _heightMap;
	_heights.clear();

	GenerateTerrain();
	UpdateMeshData();
}

void DiamondSquareTerrain::GenerateTerrain()
{
	_heightMap = new double[_info.heightMapWidth * _info.heightMapHeight];

	_info.heightMapWidth = _info.heightMapWidth;
	_info.heightMapHeight = _info.heightMapHeight;
	for (unsigned int y = 0; y < _info.heightMapHeight; y += _info.heightMapWidth / 2)
	{
		for (unsigned int x = 0; x < _info.heightMapWidth; x += _info.heightMapWidth / 2)
		{
			SetSample(x, y, MathsHandler::RandomFloat(-1.0f, 1.0f));
		}
	}

	int sampleSize = _info.heightMapWidth;
	double scale = 40.0;
	while (sampleSize > 1)
	{
		GenerateHeightmap(sampleSize, scale);
		sampleSize /= 2;
		scale /= 2.0;
	}

	_info.cellSpacing = _info.gridWidth / _info.heightMapWidth;

	UINT vertexCount = _info.heightMapWidth * _info.heightMapHeight;
	UINT faceCount = (_info.heightMapWidth - 1)*(_info.heightMapHeight - 1) * 2;

	float halfWidth = 0.5f * _info.gridWidth;
	float halfDepth = 0.5f * _info.gridDepth;

	float dx = _info.gridWidth / (_info.heightMapWidth - 1);
	float dz = _info.gridDepth / (_info.heightMapHeight - 1);

	float du = 15.0f / (_info.heightMapWidth - 1);
	float dv = 15.0f / (_info.heightMapHeight - 1);

	_gridMesh = new GridMeshData();
	_gridMesh->Vertices.resize(vertexCount);

	for (UINT i = 0; i < _info.heightMapWidth; ++i)
	{
		float z = halfDepth - i * dz;

		for (UINT j = 0; j < _info.heightMapHeight; ++j)
		{
			float x = -halfWidth + j * dx;

			_gridMesh->Vertices[i*_info.heightMapHeight + j].position = XMFLOAT3(x, static_cast<float>(_heightMap[j*_info.heightMapHeight + i]), z);
			_gridMesh->Vertices[i*_info.heightMapHeight + j].texCoord.x = j * du;
			_gridMesh->Vertices[i*_info.heightMapHeight + j].texCoord.y = i * dv;

			_gridMesh->Vertices[i*_info.heightMapHeight + j].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			_gridMesh->Vertices[i*_info.heightMapHeight + j].tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
			_gridMesh->Vertices[i*_info.heightMapHeight + j].binormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
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

	SmoothTerrain(0.5f);
	// Generate Normals
	GetNormals(faceCount, vertexCount);

	CreateBuffers();
}

HRESULT DiamondSquareTerrain::CreateBuffers()
{
	auto device = ApplicationNew::Get().GetDevice();
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

	hr = device->CreateBuffer(&vbd, &InitGridData, &_vertexBuffer);
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

	hr = device->CreateBuffer(&ibd, &InitGridIndexData, &_indexBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void DiamondSquareTerrain::UpdateMeshData()
{
	ObjectMesh updatedMesh;
	updatedMesh.vertexBuffer = GetVertexBuffer();
	updatedMesh.indexBuffer = GetIndexBuffer();
	updatedMesh.numberOfIndices = static_cast<int>(_gridMesh->Indices.size());
	updatedMesh.vertexBufferOffset = 0;
	updatedMesh.vertexBufferStride = sizeof(SimpleVertex);

	_terrainGO->GetAppearance()->UpdateObjectMesh(updatedMesh);
}

void DiamondSquareTerrain::GetNormals(UINT faceCount, UINT vertexCount)
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

void DiamondSquareTerrain::Update(float deltaTime)
{
	_terrainGO->Update(deltaTime);
}

void DiamondSquareTerrain::ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer * constBuffer)
{
	if (_finishedBuilding)
	{
		if (_terrainGO->CanCastShadows())
		{
			mb.World = XMMatrixTranspose(XMLoadFloat4x4(&_terrainGO->GetTransform()->GetWorld()));
			ApplicationNew::Get().GetContext()->UpdateSubresource(constBuffer, 0, nullptr, &mb, 0, 0);
			_terrainGO->Draw();
		}
	}
}

void DiamondSquareTerrain::Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer * matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView * texArray[])
{
	if (_finishedBuilding)
	{
		auto context = ApplicationNew::Get().GetContext();


		Transform* transform = _terrainGO->GetTransform();
		Appearance* appearance = _terrainGO->GetAppearance();

		ObjectMaterial material = appearance->GetObjectMaterial();
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

}

float DiamondSquareTerrain::GetWidth()
{
	return (_info.heightMapWidth - 1) * _info.cellSpacing;
}

float DiamondSquareTerrain::GetDepth()
{
	return (_info.heightMapHeight - 1) * _info.cellSpacing;
}

float DiamondSquareTerrain::GetHeight(float camX, float camZ)
{
	float camXWidthPos = camX + 0.5f*GetWidth();
	float camZDepthPos = camZ - 0.5f*GetDepth();

	float c = (camXWidthPos) / _info.cellSpacing;
	float d = (camZDepthPos) / -_info.cellSpacing;

	unsigned int row = static_cast<unsigned int>(floorf(d));
	unsigned int col = static_cast<unsigned int>(floorf(c));

	if (row < 0)
		row = 0;
	else if (row >= _info.heightMapHeight - 1)
		row = _info.heightMapHeight - 2;

	if (col < 0)
		col = 0;
	else if (col >= _info.heightMapWidth - 1)
		col = _info.heightMapWidth - 2;

	float A = _gridMesh->Vertices[row*_info.heightMapWidth + col].position.y;
	float B = _gridMesh->Vertices[row*_info.heightMapWidth + col + 1].position.y;
	float C = _gridMesh->Vertices[(row + 1)*_info.heightMapWidth + col].position.y;
	float D = _gridMesh->Vertices[(row + 1)*_info.heightMapWidth + col + 1].position.y;

	float s = c - static_cast<float>(col);
	float t = d - static_cast<float>(row);

	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}
