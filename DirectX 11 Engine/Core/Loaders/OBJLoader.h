#pragma once
#include <Windows.h>	//Needed for xnamath.h
#include <DirectXMath.h>	//For XMFLOAT3 and XMFLOAT2 definitions
#include <d3d11_1.h>
#include <fstream>		//For loading in an external file
#include <vector>		//For storing the XMFLOAT3/2 variables
#include <map>			//For fast searching when re-creating the index buffer
#include "../Globals/Structs.h"
#include "../Handlers/MathsHandler.h"
#include <sstream>

class [[deprecated("Use 'ModelLoader' for any model loading. That one supports multi-part objects")]] OBJLoader
{
public:
	//The only method you'll need to call
	[[deprecated("Use 'ModelLoader' for any model loading. That one supports multi-part objects")]] static ObjectMesh Load(std::wstring filename, ID3D11Device* _pd3dDevice, bool invertTexCoords = true);

private:
	//Helper methods for the above method
	//Searhes to see if a similar vertex already exists in the buffer -- if true, we re-use that index
	static bool FindSimilarVertex(const SimpleVertex& vertex, std::map<SimpleVertex, UINT>& vertToIndexMap, UINT& index);

	//Re-creates a single index buffer from the 3 given in the OBJ file
	static void CreateIndices(const std::vector<XMFLOAT3>& inVertices, const std::vector<XMFLOAT2>& inTexCoords, const std::vector<XMFLOAT3>& inNormals, const std::vector<XMFLOAT3>& inTans, const std::vector<XMFLOAT3>& inBinorms,
		std::vector<UINT>& outIndices, std::vector<XMFLOAT3>& outVertices, std::vector<XMFLOAT2>& outTexCoords, std::vector<XMFLOAT3>& outNormals,
		std::vector<XMFLOAT3>& outTangents, std::vector<XMFLOAT3>& outBinormals);

	static void CreateTangentsAndBinormals(TempVertex v1, TempVertex v2, TempVertex v3, XMFLOAT3& outTans, XMFLOAT3& outBinorms);
	static void CalculateNormal(XMFLOAT3 tangent, XMFLOAT3 binormal, XMFLOAT3 & normal);
};

	
