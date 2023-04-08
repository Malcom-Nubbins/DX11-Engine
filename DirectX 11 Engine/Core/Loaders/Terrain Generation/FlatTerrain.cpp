#include "FlatTerrain.h"
#include "../../ApplicationNew.h"

FlatTerrain::FlatTerrain() : m_TerrainGO(nullptr)
{
	m_GridVertices = std::vector<SimpleVertex>();
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_GridMesh = nullptr;
}


FlatTerrain::~FlatTerrain()
{
}

void FlatTerrain::Cleanup()
{
	if (m_VertexBuffer) m_VertexBuffer->Release();
	if (m_IndexBuffer) m_IndexBuffer->Release();
	
	delete m_GridMesh;
	m_GridMesh = nullptr;

	if (m_TerrainGO != nullptr)
	{
		m_TerrainGO->Cleanup();
		delete m_TerrainGO;
		m_TerrainGO = nullptr;
	}	
}

void FlatTerrain::SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain)
{
	m_Info.GridWidth = terrainWidth;
	m_Info.GridDepth = terrainDepth;
	m_Info.HeightMapWidth = sizeOfTerrain;
	m_Info.HeightMapHeight = sizeOfTerrain;
}

void FlatTerrain::RegenerateTerrain()
{
	// Do nothing
}

void FlatTerrain::GenerateTerrain()
{
	m_Info.CellSpacing = m_Info.GridWidth / m_Info.HeightMapHeight;

	UINT vertexCount = m_Info.HeightMapWidth * m_Info.HeightMapHeight;
	UINT faceCount = (m_Info.HeightMapWidth - 1)*(m_Info.HeightMapHeight - 1) * 2;

	float halfWidth = 0.5f * m_Info.GridWidth;
	float halfDepth = 0.5f * m_Info.GridDepth;

	float dx = m_Info.GridWidth / (m_Info.HeightMapWidth - 1);
	float dz = m_Info.GridDepth / (m_Info.HeightMapHeight - 1);

	float du = 10.0f / (m_Info.HeightMapWidth - 1);
	float dv = 10.0f / (m_Info.HeightMapHeight - 1);

	m_GridMesh = new GridMeshData();
	m_GridMesh->Vertices.resize(vertexCount);

	for (UINT i = 0; i < m_Info.HeightMapWidth; ++i)
	{
		float z = halfDepth - i * dz;

		for (UINT j = 0; j < m_Info.HeightMapHeight; ++j)
		{
			float x = -halfWidth + j * dx;

			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Position = XMFLOAT3(x, 0.0f, z);
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].TexCoord.x = j * du;
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].TexCoord.y = i * dv;

			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_GridMesh->Vertices[i*m_Info.HeightMapHeight + j].Binormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		}
	}



	for (UINT i = 0; i < m_GridMesh->Vertices.size(); ++i)
	{
		m_Heights.push_back(m_GridMesh->Vertices[i].Position.y);
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

	GetNormals(faceCount, vertexCount);
	CreateBuffers();
}

HRESULT FlatTerrain::CreateBuffers()
{
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

	hr = ApplicationNew::Get().GetDevice()->CreateBuffer(&vbd, &InitGridData, &m_VertexBuffer);
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

	hr = ApplicationNew::Get().GetDevice()->CreateBuffer(&ibd, &InitGridIndexData, &m_IndexBuffer);
	if (FAILED(hr))
		return hr;

#if defined(_DEBUG)
	char const flatTerrainVBName[] = "FlatTerrainVB";
	m_VertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(flatTerrainVBName) - 1, flatTerrainVBName);

	char const flatTerrainIBName[] = "FlatTerrainIB";
	m_IndexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(flatTerrainIBName) - 1, flatTerrainIBName);
#endif

	return S_OK;
}

void FlatTerrain::GetNormals(UINT faceCount, UINT vertexCount)
{
	for (UINT i = 0; i < faceCount; ++i)
	{
		UINT i0 = m_GridMesh->Indices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(i) * 3 + 0];
		UINT i1 = m_GridMesh->Indices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(i) * 3 + 1];
		UINT i2 = m_GridMesh->Indices[static_cast<std::vector<UINT, std::allocator<UINT>>::size_type>(i) * 3 + 2];

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

void FlatTerrain::Update(float deltaTime)
{
	m_TerrainGO->Update(deltaTime);
}

void FlatTerrain::DepthNormalDraw(MatrixBuffer mb, ID3D11Buffer * constBuffer)
{
	mb.World = XMMatrixTranspose(XMLoadFloat4x4(&m_TerrainGO->GetTransform()->GetWorld()));
	ApplicationNew::Get().GetContext()->UpdateSubresource(constBuffer, 0, nullptr, &mb, 0, 0);
	m_TerrainGO->Draw();
}

void FlatTerrain::ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer * constBuffer)
{
	mb.World = XMMatrixTranspose(XMLoadFloat4x4(&m_TerrainGO->GetTransform()->GetWorld()));
	ApplicationNew::Get().GetContext()->UpdateSubresource(constBuffer, 0, nullptr, &mb, 0, 0);
	m_TerrainGO->Draw();
}

void FlatTerrain::Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer * matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView * texArray[])
{
	auto context = ApplicationNew::Get().GetContext();

	auto* transform = m_TerrainGO->GetTransform();
	auto* appearance = m_TerrainGO->GetAppearance();

	auto material = appearance->GetObjectMaterial();

	// Copy material to shader
	cb.Surface.Ambient = material.Ambient;
	cb.Surface.Diffuse = material.Diffuse;
	cb.Surface.Specular = material.Specular;

	// Set world matrix
	mb.World = XMMatrixTranspose(XMLoadFloat4x4(&transform->GetWorld()));

	ID3D11ShaderResourceView* texture;
	// Set texture
	if (appearance->HasColourTexture())
	{
		texture = appearance->GetColourTex();
		context->PSSetShaderResources(0, 1, &texture);
		cb.UseColourTex = 1.0f;
	}
	else
	{
		cb.UseColourTex = 0.0f;
	}

	if (appearance->HasNormalMap())
	{
		cb.UseBumpMap = 1.0f;
		texture = appearance->GetNormalMap();
		context->PSSetShaderResources(2, 1, &texture);
	}
	else
	{
		cb.UseBumpMap = 0.0f;
	}

	if (m_TerrainGO->IsAffectedByLight())
	{
		cb.AffectedByLight = 1.0f;
	}
	else
	{
		cb.AffectedByLight = 0.0f;
	}

	// Update constant buffer
	context->UpdateSubresource(matrixBuffer, 0, nullptr, &mb, 0, 0);
	context->UpdateSubresource(objValuesBuffer, 0, nullptr, &cb, 0, 0);
	m_TerrainGO->Draw();
}

float FlatTerrain::GetWidth()
{
	return m_Info.GridWidth * m_TerrainGO->GetTransform()->GetScale().x;
}

float FlatTerrain::GetDepth()
{
	return m_Info.GridDepth * m_TerrainGO->GetTransform()->GetScale().z;
}

float FlatTerrain::GetHeight(float camX, float camZ)
{
	return m_TerrainGO->GetTransform()->GetPosition().y;
}
