#include "OBJLoader.h"
#include <string>

bool OBJLoader::FindSimilarVertex(const SimpleVertex& vertex, std::map<SimpleVertex, UINT>& vertToIndexMap, UINT& index)
{
	auto it = vertToIndexMap.find(vertex);
	if (it == vertToIndexMap.end())
	{
		return false;
	}
	else
	{
		index = it->second;
		return true;
	}
}

void OBJLoader::CreateIndices(
	const std::vector<XMFLOAT3>& inVertices, const std::vector<XMFLOAT2>& inTexCoords, const std::vector<XMFLOAT3>& inNormals, const std::vector<XMFLOAT3>& inTans, const std::vector<XMFLOAT3>& inBinorms,
	std::vector<UINT>& outIndices, std::vector<XMFLOAT3>& outVertices, std::vector<XMFLOAT2>& outTexCoords, std::vector<XMFLOAT3>& outNormals,
	std::vector<XMFLOAT3>& outTangents, std::vector<XMFLOAT3>& outBinormals)
{
	//Mapping from an already-existing SimpleVertex to its corresponding index
	std::map<SimpleVertex, UINT> vertToIndexMap;

	std::pair<SimpleVertex, UINT> pair;

	int numVertices = inVertices.size();
	for (int i = 0; i < numVertices; ++i) //For each vertex
	{
		SimpleVertex vertex = { inVertices[i], inNormals[i],  inTexCoords[i], inTans[i], inBinorms[i] };

		UINT index;
		bool found = FindSimilarVertex(vertex, vertToIndexMap, index); //See if a vertex already exists in the buffer that has the same attributes as this one
		if (found) //if found, re-use it's index for the index buffer
		{
			outIndices.push_back(index);

			outTangents[index] = MathsHandler::Add(outTangents[index], inTans[i]);
			outBinormals[index] = MathsHandler::Add(outBinormals[index], inBinorms[i]);
		}
		else //if not found, add it to the buffer
		{
			outVertices.push_back(vertex.pos);
			outTexCoords.push_back(vertex.texCoord);
			outNormals.push_back(vertex.normal);
			outTangents.push_back(vertex.tangent);
			outBinormals.push_back(vertex.binormal);

			UINT newIndex = static_cast<UINT>(outVertices.size()) - 1;
			outIndices.push_back(newIndex);

			//Add it to the map
			pair.first = vertex;
			pair.second = newIndex;
			vertToIndexMap.insert(pair);
		}
	}
}

//WARNING: This code makes a big assumption -- that your models have texture coordinates AND normals which they should have anyway (else you can't do texturing and lighting!)
//If your .obj file has no lines beginning with "vt" or "vn", then you'll need to change the Export settings in your modelling software so that it exports the texture coordinates 
//and normals. If you still have no "vt" lines, you'll need to do some texture unwrapping, also known as UV unwrapping.
ObjectMesh OBJLoader::Load(std::wstring filename, ID3D11Device* _pd3dDevice, bool invertTexCoords)
{
	std::wstring binaryFilename = filename;
	binaryFilename.append(L"Binary");
	std::ifstream binaryInFile;
	binaryInFile.open(binaryFilename, std::ios::in | std::ios::binary);

	if (!binaryInFile.good())
	{
		std::ifstream inFile;
		inFile.open(filename);

		if (!inFile.good())
		{
			return ObjectMesh();
		}
		else
		{
			//Vectors to store the vertex positions, normals and texture coordinates. Need to use vectors since they're resizeable and we have
			//no way of knowing ahead of time how large these meshes will be
			std::vector<XMFLOAT3> verts;
			std::vector<XMFLOAT3> normals;
			std::vector<XMFLOAT2> texCoords;

			//DirectX uses 1 index buffer, OBJ is optimized for storage and not rendering and so uses 3 smaller index buffers.....great...
			//We'll have to merge this into 1 index buffer which we'll do after loading in all of the required data.
			std::vector<UINT> vertIndices;
			std::vector<UINT> normalIndices;
			std::vector<UINT> textureIndices;

			std::string input;

			XMFLOAT3 vert;
			XMFLOAT2 texCoord;
			XMFLOAT3 normal;
			UINT vInd[3]; //indices for the vertex position
			UINT tInd[3]; //indices for the texture coordinate
			UINT nInd[3]; //indices for the normal
			std::string beforeFirstSlash;
			std::string afterFirstSlash;
			std::string afterSecondSlash;

			while (!inFile.eof()) //While we have yet to reach the end of the file...
			{
				inFile >> input; //Get the next input from the file

								 //Check what type of input it was, we are only interested in vertex positions, texture coordinates, normals and indices, nothing else
				if (input == "v") //Vertex position
				{
					inFile >> vert.x;
					inFile >> vert.y;
					inFile >> vert.z;

					verts.push_back(vert);
				}
				else if (input == "vt") //Texture coordinate
				{
					inFile >> texCoord.x;
					inFile >> texCoord.y;

					if (invertTexCoords) texCoord.y = 1.0f - texCoord.y;

					texCoords.push_back(texCoord);
				}
				else if (input == "vn") //Normal
				{
					inFile >> normal.x;
					inFile >> normal.y;
					inFile >> normal.z;

					if (invertTexCoords)
					{
						normal.x = -normal.x;
						normal.y = -normal.y;
						normal.z = -normal.z;
					}

					normals.push_back(normal);
				}
				else if (input == "f") //Face
				{
					if (invertTexCoords)
					{
						for (int i = 2; i > -1; --i)
						{
							inFile >> input;
							int slash = input.find('/'); //Find first forward slash
							int secondSlash = input.find('/', slash + 1); //Find second forward slash

																		  //Extract from string
							beforeFirstSlash = input.substr(0, slash); //The vertex position index
							afterFirstSlash = input.substr(slash + 1, secondSlash - slash - 1); //The texture coordinate index
							afterSecondSlash = input.substr(secondSlash + 1); //The normal index

																			  //Parse into int
							vInd[i] = static_cast<UINT>(atoi(beforeFirstSlash.c_str())); //atoi = "ASCII to int"
							tInd[i] = static_cast<UINT>(atoi(afterFirstSlash.c_str()));
							nInd[i] = static_cast<UINT>(atoi(afterSecondSlash.c_str()));
						}
					}
					else
					{
						for (int i = 0; i < 3; ++i)
						{
							inFile >> input;
							int slash = input.find('/'); //Find first forward slash
							int secondSlash = input.find('/', slash + 1); //Find second forward slash

																		  //Extract from string
							beforeFirstSlash = input.substr(0, slash); //The vertex position index
							afterFirstSlash = input.substr(slash + 1, secondSlash - slash - 1); //The texture coordinate index
							afterSecondSlash = input.substr(secondSlash + 1); //The normal index

																			  //Parse into int
							vInd[i] = static_cast<UINT>(atoi(beforeFirstSlash.c_str())); //atoi = "ASCII to int"
							tInd[i] = static_cast<UINT>(atoi(afterFirstSlash.c_str()));
							nInd[i] = static_cast<UINT>(atoi(afterSecondSlash.c_str()));
						}
					}

					//Place into vectors
					for (int i = 0; i < 3; ++i)
					{
						vertIndices.push_back(vInd[i] - 1);		//Minus 1 from each as these as OBJ indexes start from 1 whereas C++ arrays start from 0
						textureIndices.push_back(tInd[i] - 1);	//which is really annoying. Apart from Lua and SQL, there's not much else that has indexing 
						normalIndices.push_back(nInd[i] - 1);	//starting at 1. So many more languages index from 0, the .OBJ people screwed up there.
					}
				}
			}
			inFile.close(); //Finished with input file now, all the data we need has now been loaded in

							//Get vectors to be of same size, ready for singular indexing
			std::vector<XMFLOAT3> expandedVertices;
			std::vector<XMFLOAT3> expandedNormals;
			std::vector<XMFLOAT2> expandedTexCoords;
			unsigned int numIndices = vertIndices.size();
			for (unsigned int i = 0; i < numIndices; i++)
			{
				expandedVertices.push_back(verts[vertIndices[i]]);
				expandedTexCoords.push_back(texCoords[textureIndices[i]]);
				expandedNormals.push_back(normals[normalIndices[i]]);
			}

			//Now to (finally) form the final vertex, texture coord, normal list and single index buffer using the above expanded vectors
			std::vector<UINT> meshIndices;
			meshIndices.reserve(numIndices);
			std::vector<XMFLOAT3> meshVertices;
			meshVertices.reserve(expandedVertices.size());
			std::vector<XMFLOAT3> meshNormals;
			meshNormals.reserve(expandedNormals.size());
			std::vector<XMFLOAT2> meshTexCoords;
			meshTexCoords.reserve(expandedTexCoords.size());

			std::vector<XMFLOAT3> meshTangents;
			std::vector<XMFLOAT3> meshBinormals;

			std::vector<XMFLOAT3> finalNormals;


			TempVertex vertex1, vertex2, vertex3;
			int index = 0;

			//Turn data from vector form to arrays
			auto finalVerts = new SimpleVertex[expandedVertices.size()];
			unsigned int numMeshVertices = expandedVertices.size();

			for (int i = 0; i < numMeshVertices; i += 3)
			{
				XMFLOAT3 tangentValues = XMFLOAT3(0.0f, 0.0f, 0.0f);
				XMFLOAT3 binormalValues = XMFLOAT3(0.0f, 0.0f, 0.0f);
				XMFLOAT3 newNormalValues = XMFLOAT3(0.0f, 0.0f, 0.0f);

				vertex1.pos = expandedVertices[i + 0];
				vertex1.normal = expandedNormals[i + 0];
				vertex1.tex = expandedTexCoords[i + 0];

				vertex2.pos = expandedVertices[i + 1];
				vertex2.normal = expandedNormals[i + 1];
				vertex2.tex = expandedTexCoords[i + 1];

				vertex3.pos = expandedVertices[i + 2];
				vertex3.normal = expandedNormals[i + 2];
				vertex3.tex = expandedTexCoords[i + 2];

				CreateTangentsAndBinormals(vertex1, vertex2, vertex3, tangentValues, binormalValues);

				CalculateNormal(tangentValues, binormalValues, newNormalValues);

				meshTangents.push_back(tangentValues);
				meshTangents.push_back(tangentValues);
				meshTangents.push_back(tangentValues);

				meshBinormals.push_back(binormalValues);
				meshBinormals.push_back(binormalValues);
				meshBinormals.push_back(binormalValues);

				finalNormals.push_back(newNormalValues);
				finalNormals.push_back(newNormalValues);
				finalNormals.push_back(newNormalValues);
			}

			std::vector<XMFLOAT3> finalTangents;
			std::vector<XMFLOAT3> finalBinormals;

			CreateIndices(expandedVertices, expandedTexCoords, expandedNormals, meshTangents, meshBinormals, meshIndices, meshVertices, meshTexCoords, meshNormals, finalTangents, finalBinormals);

			numMeshVertices = meshVertices.size();

			ObjectMesh meshData;

			for (unsigned int i = 0; i < numMeshVertices; ++i)
			{
				finalVerts[i].pos = meshVertices[i];
				finalVerts[i].normal = meshNormals[i];
				finalVerts[i].texCoord = meshTexCoords[i];
				finalVerts[i].tangent = finalTangents[i];
				finalVerts[i].binormal = finalBinormals[i];
			}

			//Put data into vertex and index buffers, then pass the relevant data to the MeshData object.
			//The rest of the code will hopefully look familiar to you, as it's similar to whats in your InitVertexBuffer and InitIndexBuffer methods
			VertexBuffer* vertexBuffer;

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(SimpleVertex) * meshVertices.size();
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = finalVerts;

			_pd3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffer);

			meshData.vertexBuffer = vertexBuffer;
			meshData.vertexBufferOffset = 0;
			meshData.vertexBufferStride = sizeof(SimpleVertex);

			auto indicesArray = new UINT[meshIndices.size()];
			unsigned int numMeshIndices = meshIndices.size();
			for (unsigned int i = 0; i < numMeshIndices; ++i)
			{
				indicesArray[i] = meshIndices[i];
			}

			//Output data into binary file, the next time you run this function, the binary file will exist and will load that instead which is much quicker than parsing into vectors
			std::ofstream outbin(binaryFilename.c_str(), std::ios::out | std::ios::binary);
			outbin.write(reinterpret_cast<char*>(&numMeshVertices), sizeof(unsigned int));
			outbin.write(reinterpret_cast<char*>(&numMeshIndices), sizeof(LONG));
			outbin.write(reinterpret_cast<char*>(finalVerts), sizeof(SimpleVertex) * numMeshVertices);
			outbin.write(reinterpret_cast<char*>(indicesArray), sizeof(LONG) * numMeshIndices);
			outbin.close();

			ID3D11Buffer* indexBuffer;

			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(LONG) * meshIndices.size();
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = indicesArray;

			auto hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Error Building object", L"Error", MB_OK);
				return ObjectMesh();
			}

			meshData.numberOfIndices = meshIndices.size();
			meshData.indexBuffer = indexBuffer;

			//This data has now been sent over to the GPU so we can delete this CPU-side stuff
			delete[] indicesArray;
			delete[] finalVerts;

			return meshData;
		}
	}
	else
	{
		ObjectMesh meshData;
		unsigned int numVertices;
		unsigned int numIndices;

		//Read in array sizes
		binaryInFile.read((char*)&numVertices, sizeof(unsigned int));
		binaryInFile.read((char*)&numIndices, sizeof(unsigned int));

		//Read in data from binary file
		SimpleVertex* finalVerts = new SimpleVertex[numVertices];
		UINT* indices = new UINT[numIndices];
		binaryInFile.read((char*)finalVerts, sizeof(SimpleVertex) * numVertices);
		binaryInFile.read((char*)indices, sizeof(UINT) * numIndices);

		//Put data into vertex and index buffers, then pass the relevant data to the MeshData object.
		//The rest of the code will hopefully look familiar to you, as it's similar to whats in your InitVertexBuffer and InitIndexBuffer methods
		ID3D11Buffer* vertexBuffer;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = finalVerts;

		_pd3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffer);

		meshData.vertexBuffer = vertexBuffer;
		meshData.vertexBufferOffset = 0;
		meshData.vertexBufferStride = sizeof(SimpleVertex);

		ID3D11Buffer* indexBuffer;

		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(LONG) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = indices;
		_pd3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer);

		meshData.numberOfIndices = numIndices;
		meshData.indexBuffer = indexBuffer;

		//This data has now been sent over to the GPU so we can delete this CPU-side stuff
		delete[] indices;
		delete[] finalVerts;

		return meshData;
	}
}

void OBJLoader::CreateTangentsAndBinormals(TempVertex v1, TempVertex v2, TempVertex v3, XMFLOAT3& outTans, XMFLOAT3& outBinorms)
{
	outTans = MathsHandler::CalculateTangent(v1, v2, v3);
	outBinorms = MathsHandler::CalculateBinormal(v1, v2, v3);
}

void OBJLoader::CalculateNormal(XMFLOAT3 tangent, XMFLOAT3 binormal, XMFLOAT3& normal)
{
	float length;

	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	normal.x = -(normal.x / length);
	normal.y = -(normal.y / length);
	normal.z = -(normal.z / length);
}