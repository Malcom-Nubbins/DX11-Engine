#pragma once
#include <d3d11.h>
#include <string>
#include <tuple>
#include <fstream>
#include <istream>

#include "../Globals/Structs.h"

class ModelLoader
{
private:
	static void CalculateTBN(std::vector<SimpleVertex>& vertices, int indices[], int ids[3]);
public:
	static bool LoadModel(ID3D11Device* device, std::wstring filename, ObjectMesh& modelMesh, bool invertFaces);
};
