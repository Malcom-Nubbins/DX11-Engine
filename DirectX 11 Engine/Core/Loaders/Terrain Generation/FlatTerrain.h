#pragma once
#include <vector>
#include "../../Handlers/System Handlers/D3DClass.h"
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

	std::vector<float> _heightMap;
	std::vector<float> _heights;
	InitInfo _info;
	bool _useHeightMap;

	SceneElement* _terrainGO;

private:
	HRESULT CreateBuffers();
	void GetNormals(UINT faceCount, UINT vertexCount);

public:
	FlatTerrain(D3DClass* d3dClass);
	~FlatTerrain();
	void Cleanup();

	void SetTerrainValues(float terrainWidth, float terrainDepth, UINT sizeOfTerrain);

	void RegenerateTerrain();
	void GenerateTerrain();
	void SetGameObject(SceneElement* object) { _terrainGO = object; }

	void Update(float deltaTime);

	void DepthNormalDraw(MatrixBuffer mb, ID3D11Buffer* constBuffer);
	void ShadowDraw(ShadowDepthMatrixBuffer mb, ID3D11Buffer* constBuffer);
	void Draw(MatrixBuffer mb, ObjectValuesBuffer cb, ID3D11Buffer* matrixBuffer, ID3D11Buffer* objValuesBuffer, ID3D11ShaderResourceView* texArray[]);

	float GetWidth();
	float GetDepth();

	float GetHeight(float camX, float camZ);

	ID3D11Buffer * GetVertexBuffer() const { return _vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() const { return _indexBuffer; }
	GridMeshData* GetMeshData() const { return _gridMesh; }
};

