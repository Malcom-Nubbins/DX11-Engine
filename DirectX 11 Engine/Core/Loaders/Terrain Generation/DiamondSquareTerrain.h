#pragma once
#include "../../Scene/Scene Elements/SceneElement.h"
#include "../../Handlers/System Handlers/MathsHandler.h"
#include <vector>
#include <fstream>

class DiamondSquareTerrain
{
	struct GridMeshData
	{
		std::vector<TerrainVertex> Vertices;
		std::vector<UINT> Indices;
	};

	struct InitInfo
	{
		std::wstring HeightMapFileName;
		UINT HeightMapHeight = 0;
		UINT HeightMapWidth = 0;

		float GridWidth = 0.f;
		float GridDepth = 0.f;
		float HeightScale = 0.f;

		float CellSpacing = 1.f;
	};

private:
	TVertexBufferPtr m_VertexBuffer;
	TIndexBufferPtr m_IndexBuffer;

	GridMeshData* m_GridMesh;

	std::vector<SimpleVertex> m_GridVertices;
	std::vector<LONG> m_GridIndices;

	double* m_HeightMap;
	std::vector<float> m_Heights;
	InitInfo m_Info;

	bool m_bFinishedBuilding;

	static const int ms_CellsPerPatch = 64;

private:

	void GenerateHeightmap(int sampleSize, double scale);

	void SampleSquare(int x, int y, int size, double value);

	void SampleDiamond(int x, int y, int size, double value);

	double Sample(int x, int y);

	void SetSample(int x, int y, double value);

	void SmoothTerrain(float smoothnessAmount);

	HRESULT CreateBuffers();

	void GetNormals(UINT faceCount, UINT vertexCount);

public:
	DiamondSquareTerrain();
	~DiamondSquareTerrain();
	void Cleanup();

	void SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain);
	void RegenerateTerrain();
	void GenerateTerrain();

	void Update(float deltaTime);

	void ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer* constBuffer);
	void Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer* matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView* texArray[]);

public:
	float GetWidth();
	float GetDepth();
	float GetHeight(float camX, float camZ);

	TVertexBufferPtr GetVertexBuffer() const { return m_VertexBuffer; }
	TVertexBufferPtr GetIndexBuffer() const { return m_IndexBuffer; }
	GridMeshData* GetMeshData() const { return m_GridMesh; }

	void SetTerrainFinishedBuilding(bool state) { m_bFinishedBuilding = state; }
	bool HasTerrainFinishedBuilding() const { return m_bFinishedBuilding; }
	void UpdateMeshData();

};

