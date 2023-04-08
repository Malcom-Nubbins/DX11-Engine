#include "DiamondSquareTerrain.h"
#include "../../ApplicationNew.h"

DiamondSquareTerrain::DiamondSquareTerrain()
{
	m_GridVertices = std::vector<SimpleVertex>();
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_GridMesh = nullptr;
	m_bFinishedBuilding = false;
}

DiamondSquareTerrain::~DiamondSquareTerrain()
{
}

void DiamondSquareTerrain::Cleanup()
{
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_IndexBuffer) m_IndexBuffer->Release();
	delete m_GridMesh;
	m_GridMesh = nullptr;

	delete[] m_HeightMap;
}

void DiamondSquareTerrain::GenerateHeightmap(int sampleSize, double scale)
{
	int halfStep = sampleSize / 2;

	for (unsigned int y = halfStep; y < m_Info.HeightMapHeight + halfStep; y += sampleSize)
	{
		for (unsigned int x = halfStep; x < m_Info.HeightMapWidth + halfStep; x += sampleSize)
		{
			SampleSquare(x, y, sampleSize, MathsHandler::RandomFloat(-1.0f, 1.0f) * scale);
		}
	}

	for (unsigned int y = 0; y < m_Info.HeightMapHeight; y += sampleSize)
	{
		for (unsigned int x = 0; x < m_Info.HeightMapWidth; x += sampleSize)
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
	return m_HeightMap[((x & (m_Info.HeightMapWidth - 1)) + (y & (m_Info.HeightMapHeight - 1)) * m_Info.HeightMapWidth)];
}

void DiamondSquareTerrain::SetSample(int x, int y, double value)
{
	if (value >= 0.0f)
		m_HeightMap[(x & (m_Info.HeightMapWidth - 1)) + (y & (m_Info.HeightMapHeight - 1)) * m_Info.HeightMapWidth] = value;
	else
		m_HeightMap[(x & (m_Info.HeightMapWidth - 1)) + (y & (m_Info.HeightMapHeight - 1)) * m_Info.HeightMapWidth] = 0.0f;
}

void DiamondSquareTerrain::SmoothTerrain(float smoothnessAmount)
{
	uint64 x, z;

	for (x = 1; x < m_Info.HeightMapWidth; ++x)
	{
		for (z = 0; z < m_Info.HeightMapHeight; ++z)
		{
			m_GridMesh->Vertices[x * m_Info.HeightMapHeight + z].Position.y =
				m_GridMesh->Vertices[(x - 1)*m_Info.HeightMapHeight + z].Position.y * (1 - smoothnessAmount) +
				m_GridMesh->Vertices[x*m_Info.HeightMapHeight + z].Position.y * smoothnessAmount;
		}
	}

	for (x = static_cast<uint64>(m_Info.HeightMapWidth) - 2; x > 0; --x)
	{
		for (z = 0; z < m_Info.HeightMapHeight; ++z)
		{
			m_GridMesh->Vertices[x * m_Info.HeightMapHeight + z].Position.y =
				m_GridMesh->Vertices[(x + 1)*m_Info.HeightMapHeight + z].Position.y * (1 - smoothnessAmount) +
				m_GridMesh->Vertices[x*m_Info.HeightMapHeight + z].Position.y * smoothnessAmount;
		}
	}

	for (x = 0; x < m_Info.HeightMapWidth; ++x)
	{
		for (z = 1; z < m_Info.HeightMapHeight; ++z)
		{
			m_GridMesh->Vertices[x * m_Info.HeightMapHeight + z].Position.y =
				m_GridMesh->Vertices[x*m_Info.HeightMapHeight + (z - 1)].Position.y * (1 - smoothnessAmount) +
				m_GridMesh->Vertices[x*m_Info.HeightMapHeight + z].Position.y * smoothnessAmount;
		}
	}

	for (x = 0; x < m_Info.HeightMapWidth; ++x)
	{
		for (z = static_cast<uint64>(m_Info.HeightMapHeight) - 1; z > 0; --z)
		{
			const uint64 nextZIdx = x * m_Info.HeightMapHeight + (z + 1);
			const uint64 nextXIdx = (x + 1) * m_Info.HeightMapHeight + z;

			if (nextZIdx < m_GridMesh->Vertices.size() && nextXIdx < m_GridMesh->Vertices.size())
			{
				m_GridMesh->Vertices[x * m_Info.HeightMapHeight + z].Position.y =
					m_GridMesh->Vertices[nextZIdx].Position.y * (1 - smoothnessAmount) +
					m_GridMesh->Vertices[nextXIdx].Position.y * smoothnessAmount;
			}	
		}
	}
}

void DiamondSquareTerrain::SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain)
{
	m_Info.GridWidth = terrainWidth;
	m_Info.GridDepth = terrainDepth;
	m_Info.HeightMapWidth = sizeOfTerrain;
	m_Info.HeightMapHeight = sizeOfTerrain;
}

void DiamondSquareTerrain::RegenerateTerrain()
{
	m_VertexBuffer->Release();
	m_IndexBuffer->Release();
	m_GridVertices.clear();
	m_GridIndices.clear();
	delete m_GridMesh;
	delete[] m_HeightMap;
	m_Heights.clear();

	GenerateTerrain();
	UpdateMeshData();
}

void DiamondSquareTerrain::GenerateTerrain()
{
	m_HeightMap = new double[m_Info.HeightMapWidth * m_Info.HeightMapHeight];

	m_Info.HeightMapWidth = m_Info.HeightMapWidth;
	m_Info.HeightMapHeight = m_Info.HeightMapHeight;
	for (unsigned int y = 0; y < m_Info.HeightMapHeight; y += m_Info.HeightMapWidth / 2)
	{
		for (unsigned int x = 0; x < m_Info.HeightMapWidth; x += m_Info.HeightMapWidth / 2)
		{
			SetSample(x, y, MathsHandler::RandomFloat(-1.0f, 1.0f));
		}
	}

	int sampleSize = m_Info.HeightMapWidth;
	double scale = 40.0;
	while (sampleSize > 1)
	{
		GenerateHeightmap(sampleSize, scale);
		sampleSize /= 2;
		scale /= 2.0;
	}

	m_Info.CellSpacing = m_Info.GridWidth / m_Info.HeightMapWidth;

	UINT vertexCount = m_Info.HeightMapWidth * m_Info.HeightMapHeight;
	UINT faceCount = (m_Info.HeightMapWidth - 1)*(m_Info.HeightMapHeight - 1) * 2;

	float halfWidth = 0.5f * m_Info.GridWidth;
	float halfDepth = 0.5f * m_Info.GridDepth;

	float dx = m_Info.GridWidth / (m_Info.HeightMapWidth - 1);
	float dz = m_Info.GridDepth / (m_Info.HeightMapHeight - 1);

	float du = 15.0f / (m_Info.HeightMapWidth - 1);
	float dv = 15.0f / (m_Info.HeightMapHeight - 1);

	m_GridMesh = new GridMeshData();
	m_GridMesh->Vertices.resize(vertexCount);

	for (UINT i = 0; i < m_Info.HeightMapWidth; ++i)
	{
		float z = halfDepth - i * dz;

		for (UINT j = 0; j < m_Info.HeightMapHeight; ++j)
		{
			float x = -halfWidth + j * dx;

			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Position = XMFLOAT3(x, static_cast<float>(m_HeightMap[j*m_Info.HeightMapHeight + i]), z);
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].TexCoord.x = j * du;
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].TexCoord.y = i * dv;

			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Binormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
	}

	m_GridMesh->Indices.resize(faceCount * 3);

	UINT k = 0;
	for (UINT i = 0; i < m_Info.HeightMapWidth - 1; ++i)
	{
		for (UINT j = 0; j < m_Info.HeightMapHeight - 1; ++j)
		{
			m_GridMesh->Indices[k] = i * m_Info.HeightMapHeight + j;
			m_GridMesh->Indices[k + 1] = i * m_Info.HeightMapHeight + j + 1;
			m_GridMesh->Indices[k + 2] = (i + 1)*m_Info.HeightMapHeight + j;
			m_GridMesh->Indices[k + 3] = (i + 1)*m_Info.HeightMapHeight + j;
			m_GridMesh->Indices[k + 4] = i * m_Info.HeightMapHeight + j + 1;
			m_GridMesh->Indices[k + 5] = (i + 1)*m_Info.HeightMapHeight + j + 1;
			k += 6;
		}
	}

	SmoothTerrain(0.7f);
	// Generate Normals
	GetNormals(faceCount, vertexCount);

	CreateBuffers();
}

HRESULT DiamondSquareTerrain::CreateBuffers()
{
	auto device = ApplicationNew::Get().GetDevice();
	HRESULT hr;
	// Vertex Buffer
	m_GridVertices.resize(m_GridMesh->Vertices.size());
	UINT k = 0;

	for (size_t i = 0; i < m_GridMesh->Vertices.size(); i++, k++)
	{
		m_GridVertices[k].Pos = m_GridMesh->Vertices[i].Position;
		m_GridVertices[k].Normal = m_GridMesh->Vertices[i].Normal;
		m_GridVertices[k].TexCoord = m_GridMesh->Vertices[i].TexCoord;
		m_GridVertices[k].Tangent = m_GridMesh->Vertices[i].Tangent;
		m_GridVertices[k].Binormal = m_GridMesh->Vertices[i].Binormal;
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(SimpleVertex) * static_cast<UINT>(m_GridMesh->Vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGridData;
	ZeroMemory(&InitGridData, sizeof(InitGridData));
	InitGridData.pSysMem = &m_GridVertices[0];

	hr = device->CreateBuffer(&vbd, &InitGridData, &m_VertexBuffer);
	if (FAILED(hr))
	{
		return hr;
	}

	// Index Buffer
	m_GridIndices.insert(m_GridIndices.end(), m_GridMesh->Indices.begin(), m_GridMesh->Indices.end());

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(LONG) * static_cast<UINT>(m_GridMesh->Indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGridIndexData;
	ZeroMemory(&InitGridIndexData, sizeof(InitGridIndexData));
	InitGridIndexData.pSysMem = &m_GridIndices[0];

	hr = device->CreateBuffer(&ibd, &InitGridIndexData, &m_IndexBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void DiamondSquareTerrain::UpdateMeshData()
{
	ObjectMesh updatedMesh;
	updatedMesh.VertexBuffer = GetVertexBuffer();
	updatedMesh.IndexBuffer = GetIndexBuffer();
	updatedMesh.NumberOfIndices = static_cast<int>(m_GridMesh->Indices.size());
	updatedMesh.VertexBufferOffset = 0;
	updatedMesh.vertexBufferStride = sizeof(SimpleVertex);
}

void DiamondSquareTerrain::GetNormals(UINT faceCount, UINT vertexCount)
{
	for (UINT i = 0; i < faceCount; ++i)
	{
		UINT i0 = m_GridMesh->Indices[i * 3 + 0];
		UINT i1 = m_GridMesh->Indices[i * 3 + 1];
		UINT i2 = m_GridMesh->Indices[i * 3 + 2];

		TempVertex v1, v2, v3;
		v1.Pos = m_GridMesh->Vertices[i0].Position;
		v2.Pos = m_GridMesh->Vertices[i1].Position;
		v3.Pos = m_GridMesh->Vertices[i2].Position;

		XMFLOAT3 e0Float = MathsHandler::Subtract(v2.Pos, v1.Pos);
		XMFLOAT3 e1Float = MathsHandler::Subtract(v3.Pos, v1.Pos);
		XMVECTOR e0 = XMLoadFloat3(&e0Float);
		XMVECTOR e1 = XMLoadFloat3(&e1Float);
		XMVECTOR normal = XMVector3Cross(e0, e1);
		XMFLOAT3 normalFloat;
		XMStoreFloat3(&normalFloat, normal);

		m_GridMesh->Vertices[i0].Normal = MathsHandler::Add(m_GridMesh->Vertices[i0].Normal, normalFloat);
		m_GridMesh->Vertices[i1].Normal = MathsHandler::Add(m_GridMesh->Vertices[i1].Normal, normalFloat);
		m_GridMesh->Vertices[i2].Normal = MathsHandler::Add(m_GridMesh->Vertices[i2].Normal, normalFloat);

		v1.Pos = m_GridMesh->Vertices[i0].Position;
		v1.Normal = m_GridMesh->Vertices[i0].Normal;
		v1.TexCoord = m_GridMesh->Vertices[i0].TexCoord;

		v2.Pos = m_GridMesh->Vertices[i1].Position;
		v2.Normal = m_GridMesh->Vertices[i1].Normal;
		v2.TexCoord = m_GridMesh->Vertices[i1].TexCoord;

		v3.Pos = m_GridMesh->Vertices[i2].Position;
		v3.Normal = m_GridMesh->Vertices[i2].Normal;
		v3.TexCoord = m_GridMesh->Vertices[i2].TexCoord;

		m_GridMesh->Vertices[i0].Tangent = MathsHandler::CalculateTangent(v1, v2, v3);
		m_GridMesh->Vertices[i1].Tangent = MathsHandler::CalculateTangent(v1, v2, v3);
		m_GridMesh->Vertices[i2].Tangent = MathsHandler::CalculateTangent(v1, v2, v3);

		m_GridMesh->Vertices[i0].Binormal = MathsHandler::CalculateBinormal(v1, v2, v3);
		m_GridMesh->Vertices[i1].Binormal = MathsHandler::CalculateBinormal(v1, v2, v3);
		m_GridMesh->Vertices[i2].Binormal = MathsHandler::CalculateBinormal(v1, v2, v3);

	}

	for (UINT i = 0; i < vertexCount; ++i)
	{
		XMVECTOR normal = XMLoadFloat3(&m_GridMesh->Vertices[i].Normal);
		XMVector3Normalize(normal);
		XMStoreFloat3(&m_GridMesh->Vertices[i].Normal, normal);
	}
}

void DiamondSquareTerrain::Update(float deltaTime)
{
	
}

void DiamondSquareTerrain::ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer * constBuffer)
{
	
}

void DiamondSquareTerrain::Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer * matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView * texArray[])
{
	
}

float DiamondSquareTerrain::GetWidth()
{
	return (m_Info.HeightMapWidth - 1) * m_Info.CellSpacing;
}

float DiamondSquareTerrain::GetDepth()
{
	return (m_Info.HeightMapHeight - 1) * m_Info.CellSpacing;
}

float DiamondSquareTerrain::GetHeight(float camX, float camZ)
{
	float camXWidthPos = camX + 0.5f*GetWidth();
	float camZDepthPos = camZ - 0.5f*GetDepth();

	float c = (camXWidthPos) / m_Info.CellSpacing;
	float d = (camZDepthPos) / -m_Info.CellSpacing;

	unsigned int row = static_cast<unsigned int>(floorf(d));
	unsigned int col = static_cast<unsigned int>(floorf(c));

	if (row < 0)
		row = 0;
	else if (row >= m_Info.HeightMapHeight - 1)
		row = m_Info.HeightMapHeight - 2;

	if (col < 0)
		col = 0;
	else if (col >= m_Info.HeightMapWidth - 1)
		col = m_Info.HeightMapWidth - 2;

	float A = m_GridMesh->Vertices[row*m_Info.HeightMapWidth + col].Position.y;
	float B = m_GridMesh->Vertices[row*m_Info.HeightMapWidth + col + 1].Position.y;
	float C = m_GridMesh->Vertices[(row + 1)*m_Info.HeightMapWidth + col].Position.y;
	float D = m_GridMesh->Vertices[(row + 1)*m_Info.HeightMapWidth + col + 1].Position.y;

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
