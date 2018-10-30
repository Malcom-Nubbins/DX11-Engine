#pragma once
#include "../../Handlers/System Handlers/D3DClass.h"
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
		std::wstring heightMapFileName;
		UINT heightMapHeight;
		UINT heightMapWidth;

		float gridWidth;
		float gridDepth;
		float heightScale;

		float cellSpacing;
	};

private:
	D3DClass * _d3dClass;

	ID3D11Buffer * _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	GridMeshData* _gridMesh;

	std::vector<SimpleVertex> _gridVertices;
	std::vector<LONG> _gridIndices;

	double* _heightMap;
	std::vector<float> _heights;
	InitInfo _info;

	SceneElement *_terrainGO;

	bool _finishedBuilding;

	static const int _cellsPerPatch = 64;

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
	DiamondSquareTerrain(D3DClass* d3dClass);
	~DiamondSquareTerrain();
	void Cleanup();

	void SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain);
	void RegenerateTerrain();
	void GenerateTerrain();
	void SetGameObject(SceneElement* object) { _terrainGO = object; }

	void Update(float deltaTime);

	void ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer* constBuffer);
	void Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer* matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView* texArray[]);

public:
	float GetWidth();
	float GetDepth();
	float GetHeight(float camX, float camZ);

	ID3D11Buffer * GetVertexBuffer() const { return _vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() const { return _indexBuffer; }
	GridMeshData* GetMeshData() const { return _gridMesh; }

	void SetTerrainFinishedBuilding(bool state) { _finishedBuilding = state; }
	bool HasTerrainFinishedBuilding() const { return _finishedBuilding; }
	void UpdateMeshData();

};

