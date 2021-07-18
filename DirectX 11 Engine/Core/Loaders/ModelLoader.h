#pragma once
#include <d3d11.h>
#include <string>
#include <tuple>
#include <fstream>
#include <istream>

#include "../Globals/Structs.h"
#include "../Handlers/System Handlers/MathsHandler.h"
#include "../Globals/AppValues.h"
#include <map>

class ModelLoader
{
private:
	static std::map<std::wstring, NewObjectMesh> _modelCache;

	struct Tokens
	{
		char** tokensList;
		size_t count;
	};

	struct IntegerTokens
	{
		int** tokensList;
		size_t count;
	};

private:
	static std::vector<std::string> Split(const std::string& text, const std::string& delims);
	static void CalculateTBN(std::vector<SimpleVertex>& vertices, int** indices, int ids[3]);
public:
	static bool LoadModel(ID3D11Device* device, std::wstring filename, NewObjectMesh& modelMesh, bool invertFaces);

	static void UnloadAllModels();
};

