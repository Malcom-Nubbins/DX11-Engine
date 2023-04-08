#pragma once
#include <vector>
#include "../../Globals/Structs.h"
#include "../../Scene/Scene Elements/SceneElement.h"
#include "../../Handlers/System Handlers/MathsHandler.h"

class FlatTerrain
{
	struct GridMeshData
	{
		std::vector<TerrainVertex> Vertices;
		std::vector<UINT> Indices;
	};

	struct InitInfo
	{
		std::wstring HeightMapFileName;
		UINT HeightMapHeight;
		UINT HeightMapWidth;

		float GridWidth;
		float GridDepth;
		float HeightScale;

		float CellSpacing;
	};
private:
	TVertexBufferPtr m_VertexBuffer;
	TIndexBufferPtr m_IndexBuffer;

	GridMeshData* m_GridMesh;

	std::vector<SimpleVertex> m_GridVertices;
	std::vector<LONG> m_GridIndices;

	std::vector<float> m_HeightMap;
	std::vector<float> m_Heights;
	InitInfo m_Info;
	bool m_bUseHeightMap;

	SceneElement* m_TerrainGO;

private:
	HRESULT CreateBuffers();
	void GetNormals(UINT faceCount, UINT vertexCount);

public:
	FlatTerrain();
	~FlatTerrain();
	void Cleanup();

	void SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain);

	void RegenerateTerrain();
	void GenerateTerrain();
	void SetGameObject(SceneElement* object) { m_TerrainGO = object; }

	void Update(float deltaTime);

	void DepthNormalDraw(MatrixBuffer mb, ID3D11Buffer* constBuffer);
	void ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer* constBuffer);
	void Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer* matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView* texArray[]);

	float GetWidth();
	float GetDepth();

	float GetHeight(float camX, float camZ);

	TVertexBufferPtr GetVertexBuffer() const { return m_VertexBuffer; }
	TIndexBufferPtr GetIndexBuffer() const { return m_IndexBuffer; }
	GridMeshData* GetMeshData() const { return m_GridMesh; }
};

