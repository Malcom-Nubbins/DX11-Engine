#include "ModelLoader.h"
#include <iterator>
#include <map>

std::map<std::wstring, NewObjectMesh> ModelLoader::_modelCache;

std::vector<std::string> ModelLoader::Split(const std::string& text, const std::string& delims)
{
	return std::vector<std::string>();
}


void ModelLoader::CalculateTBN(std::vector<SimpleVertex>& vertices, int** indices, int ids[3])
{
	SimpleVertex& v0 = vertices[*indices[ids[0]]];
	SimpleVertex& v1 = vertices[*indices[ids[1]]];
	SimpleVertex& v2 = vertices[*indices[ids[2]]];

	XMVECTOR A = XMLoadFloat3(&v1.pos) - XMLoadFloat3(&v0.pos);
	XMVECTOR B = XMLoadFloat3(&v2.pos) - XMLoadFloat3(&v0.pos);

	XMVECTOR P = XMLoadFloat2(&v1.texCoord) - XMLoadFloat2(&v0.texCoord);
	XMVECTOR Q = XMLoadFloat2(&v2.texCoord) - XMLoadFloat2(&v0.texCoord);

	XMFLOAT2 fP, fQ;
	XMStoreFloat2(&fP, P); XMStoreFloat2(&fQ, Q);
	float fraction = 1.0f / (fP.x * fQ.y - fQ.x * fP.y);

	XMFLOAT3 output[3];
	XMStoreFloat3(&output[2], XMVector3Normalize(XMVector3Cross(A, B)));

	XMFLOAT3 fA, fB;
	XMStoreFloat3(&fA, A); XMStoreFloat3(&fB, B);

	// tangent
	output[0] = XMFLOAT3((fQ.y * fA.x - fP.y * fB.x) * fraction,
		(fQ.y * fA.y - fP.y * fB.y) * fraction,
		(fQ.y * fA.z - fP.y * fB.z) * fraction);

	float len = sqrt(output[0].x*output[0].x + output[0].y*output[0].y + output[0].z*output[0].z);
	if (len > 0.0001f)
	{
		output[0].x /= len; output[0].y /= len; output[0].z /= len;
	}

	// binormal
	output[1] = XMFLOAT3((fP.x * fB.x - fQ.x * fA.x) * fraction,
		(fP.x * fB.y - fQ.x * fA.y) * fraction,
		(fP.x * fB.z - fQ.x * fA.z) * fraction);

	len = sqrt(output[1].x*output[1].x + output[1].y*output[1].y + output[1].z*output[1].z);
	if (len > 0.0001f)
	{
		output[1].x /= len; output[1].y /= len; output[1].z /= len;
	}

	XMVECTOR vOutput[3];
	vOutput[0] = XMLoadFloat3(&output[0]);
	vOutput[1] = XMLoadFloat3(&output[1]);
	vOutput[2] = XMLoadFloat3(&output[2]);

	if (XMVectorGetX(XMVector3Dot(vOutput[2], XMVector3Cross(vOutput[0], vOutput[1]))) < 0)
	{
		vOutput[0] = XMVectorNegate(vOutput[0]);
	}

	float NdotT = XMVectorGetX(XMVector3Dot(vOutput[2], vOutput[0]));
	float NdotB = XMVectorGetX(XMVector3Dot(vOutput[2], vOutput[1]));
	float TdotB = XMVectorGetX(XMVector3Dot(vOutput[0], vOutput[1]));

	vOutput[0] = vOutput[0] - NdotT * vOutput[2];
	vOutput[1] = vOutput[1] - NdotB * vOutput[2] - TdotB * vOutput[0];

	XMStoreFloat3(&v0.normal, vOutput[2]);
	XMStoreFloat3(&v0.tangent, vOutput[0]);
	XMStoreFloat3(&v0.binormal, vOutput[1]);
}

bool ModelLoader::LoadModel(ID3D11Device* device, std::wstring filename, 
							NewObjectMesh& modelMesh, bool invertFaces)
{
	if(_modelCache.find(filename) != _modelCache.end())
	{
		modelMesh.vertexBuffer = _modelCache[filename].vertexBuffer;
		modelMesh.numOfSubsets = _modelCache[filename].numOfSubsets;
		modelMesh.subsets = _modelCache[filename].subsets;
		return true;
	}

	modelMesh = NewObjectMesh();
	modelMesh.numOfSubsets = 0;

	/*std::wstring binaryFilename = filename;
	binaryFilename.append(L"Binary");

	std::ifstream binaryInFile;
	binaryInFile.open(binaryFilename, std::ios::in | std::ios::binary);
	if(binaryInFile.good())
	{
		
		return true;
	}*/


	std::ifstream in;
	in.open(filename);

	if(!in.good())
	{
		return false;
	}

	typedef std::tuple<int, int, int> uniqueVertex;
	std::map<uniqueVertex, int> availableVertices;

	std::vector<XMFLOAT3> verts;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texCoords;
	std::vector<XMFLOAT4> tangents;
	std::vector<XMFLOAT3> binormals;

	std::vector<SimpleVertex> vertices;
	std::vector<Subset> subsets;
	std::vector<UINT> indices = std::vector<UINT>();

	UINT vertexOffset = 0;
	UINT currId = 0;

	std::string ignore;
	std::string currLine;

	XMFLOAT3 vert;
	XMFLOAT3 norm;
	XMFLOAT2 texCoord;

	auto buildSubset = [&]() -> bool
	{
		if (indices.empty())
			return true;

		Subset subset = Subset();
		subset.id = currId;


		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(LONG) * indices.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(initData));
		initData.pSysMem = &indices[0];

		auto hr = device->CreateBuffer(&bd, &initData, &subset.indexBuffer);
		if (FAILED(hr))
			return false;

		subset.vertexBufferStride = sizeof(SimpleVertex);
		subset.vertexBufferOffset = 0;
		subset.indexCount = indices.size();

		modelMesh.subsets.push_back(subset);
		modelMesh.numOfSubsets++;

		vertexOffset += indices.size();
		indices.clear();

		currId++;

		return true;
	};

	auto parseVertex = [&](char** tokens, size_t size) -> bool
	{
		if (currLine[1] == 'n')
		{
			// parse normal
			if (!buildSubset())
				return false;

			if (size != 4)
				return false;

			norm.x = static_cast<float>(atof(tokens[1]));
			norm.y = static_cast<float>(atof(tokens[2]));
			norm.z = static_cast<float>(atof(tokens[3]));

			if(invertFaces)
			{
				norm.x = -norm.x;
				norm.y = -norm.y;
				norm.z = -norm.z;
			}

			normals.push_back(norm);

		}
		else if (currLine[1] == 't')
		{
			// parse texcoord
			if (!buildSubset())
				return false;

			if (size < 3)
				return false;

			texCoord.x = static_cast<float>(atof(tokens[1]));
			texCoord.y = static_cast<float>(atof(tokens[2]));

			if(invertFaces)
			{
				texCoord.y = 1.0f - texCoord.y;
			}

			texCoords.push_back(texCoord);
		}
		else
		{
			// parse vertex
			if (!buildSubset())
				return false;

			if (size != 4)
				return false;

			vert.x = static_cast<float>(atof(tokens[1]));
			vert.y = static_cast<float>(atof(tokens[2]));
			vert.z = static_cast<float>(atof(tokens[3]));

			verts.push_back(vert);
		}
		return true;
	};

	auto parseFace = [&](char** tokens,
		size_t size) -> bool
	{
		if (size < 4)
		{
			return false;
		}

		IntegerTokens intTokens;
		intTokens.tokensList = static_cast<int**>(malloc(size * sizeof(*intTokens.tokensList)));
		if(!intTokens.tokensList)
		{
			return false;
		}

		intTokens.count = 0;

		Tokens faceTokens;
		faceTokens.tokensList = static_cast<char**>(malloc(size * sizeof(*faceTokens.tokensList)));
		if(!faceTokens.tokensList)
		{
			return false;
		}

		faceTokens.count = 0;

		if(invertFaces)
		{
			for (size_t i = size - 1; i > 0; --i)
			{
				const std::string& face = tokens[i];
				if (face.empty())
					continue;

				char* cstr = new char[face.length() + 1];
				strcpy(cstr, face.c_str());

				char* p = std::strtok(cstr, "/");

				size_t count = 0;
				for (UINT ui = 0; p != nullptr; ++ui)
				{
					faceTokens.tokensList[ui] = static_cast<char*>(malloc(strlen(p) + 1));
					if (faceTokens.tokensList[ui])
					{
						strcpy(faceTokens.tokensList[ui], p);
						p = std::strtok(nullptr, "/");
						count++;
					}
					else
					{
						return false;
					}
				}

				faceTokens.count = count;

				delete[] cstr;

				int idPos = -1, idTex = -1, idNorm = -1;

				idPos = static_cast<int>(atoi(faceTokens.tokensList[0]));
				if (faceTokens.tokensList[1] != nullptr)
					idTex = static_cast<int>(atoi(faceTokens.tokensList[1]));

				idNorm = static_cast<int>(atoi(faceTokens.tokensList[2]));

				--idPos; --idTex; --idNorm;

				int vertexIndex = -1;
				const auto ids = std::make_tuple(idPos, idTex, idNorm);
				const auto avit = availableVertices.find(ids);
				if (avit != availableVertices.end())
				{
					vertexIndex = avit->second;
				}
				else
				{
					SimpleVertex v{};

					v.pos = verts[idPos];
					if (idTex >= 0)
					{
						v.texCoord = texCoords[idTex];
					}
					else
					{
						v.texCoord = XMFLOAT2(0.0f, 0.0f);
					}

					v.normal = normals[idNorm];

					vertices.push_back(v);
					vertexIndex = vertices.size() - 1;
					availableVertices.insert(std::make_pair(std::make_tuple(idPos, idTex, idNorm), vertexIndex));
				}

				if (i < 4)
				{
					indices.push_back(vertexIndex);

					intTokens.tokensList[i - 1] = static_cast<int*>(malloc(sizeof(vertexIndex)));
					if (!intTokens.tokensList[i - 1])
					{
						return false;
					}

					*intTokens.tokensList[i - 1] = vertexIndex;
					intTokens.count++;
				}
				else
				{
					indices.push_back(vertexIndex);
					indices.push_back(*intTokens.tokensList[0]);
					indices.push_back(*intTokens.tokensList[2]);

					int* lIndices = new int[3]{ vertexIndex, *intTokens.tokensList[0], *intTokens.tokensList[2] };
					int inds[] = { 0, 1, 2 };
					CalculateTBN(vertices, &lIndices, inds);

					delete[] lIndices;
				}
			}
		}
		else
		{
			for (size_t i = 1; i < size; ++i)
			{
				const std::string& face = tokens[i];
				if (face.empty())
					continue;

				char* cstr = new char[face.length() + 1];
				strcpy(cstr, face.c_str());

				char* p = std::strtok(cstr, "/");

				size_t count = 0;
				for (UINT ui = 0; p != nullptr; ++ui)
				{
					faceTokens.tokensList[ui] = static_cast<char*>(malloc(strlen(p) + 1));
					if (faceTokens.tokensList[ui])
					{
						strcpy(faceTokens.tokensList[ui], p);
						p = std::strtok(nullptr, "/");
						count++;
					}
					else
					{
						return false;
					}
				}

				faceTokens.count = count;

				delete[] cstr;

				int idPos = -1, idTex = -1, idNorm = -1;

				idPos = static_cast<int>(atoi(faceTokens.tokensList[0]));
				if (faceTokens.tokensList[1] != nullptr)
					idTex = static_cast<int>(atoi(faceTokens.tokensList[1]));

				idNorm = static_cast<int>(atoi(faceTokens.tokensList[2]));

				--idPos; --idTex; --idNorm;

				int vertexIndex = -1;
				const auto ids = std::make_tuple(idPos, idTex, idNorm);
				const auto avit = availableVertices.find(ids);
				if (avit != availableVertices.end())
				{
					vertexIndex = avit->second;
				}
				else
				{
					SimpleVertex v{};

					v.pos = verts[idPos];
					if (idTex >= 0)
					{
						v.texCoord = texCoords[idTex];
					}
					else
					{
						v.texCoord = XMFLOAT2(0.0f, 0.0f);
					}

					v.normal = normals[idNorm];

					vertices.push_back(v);
					vertexIndex = vertices.size() - 1;
					availableVertices.insert(std::make_pair(std::make_tuple(idPos, idTex, idNorm), vertexIndex));
				}

				if (i < 4)
				{
					indices.push_back(vertexIndex);
					intTokens.tokensList[i - 1] = static_cast<int*>(malloc(sizeof(vertexIndex)));
					if (!intTokens.tokensList[i - 1])
					{
						return false;
					}

					//memcpy(&intTokens.tokensList[i - 1], &vertexIndex, sizeof(int));

					*intTokens.tokensList[i - 1] = vertexIndex;
					intTokens.count++;
				}
				else
				{
					indices.push_back(vertexIndex);
					indices.push_back(*intTokens.tokensList[0]);
					indices.push_back(*intTokens.tokensList[2]);

					//int lIndices[] = { vertexIndex, *intTokens.tokensList[0], *intTokens.tokensList[2] };

					int* lIndices = new int[3]{vertexIndex, *intTokens.tokensList[0], *intTokens.tokensList[2] };

					int inds[] = { 0, 1, 2 };
					CalculateTBN(vertices, &lIndices, inds);

					delete[] lIndices;
				}

			}
		}

		for (UINT i = 0; i < faceTokens.count; ++i)
		{
			free(faceTokens.tokensList[i]);
		}

		free(faceTokens.tokensList);

		int lIndices[] = { 2, 0, 1 };

		for (int i = 0; i < 3; ++i)
		{
			int first = lIndices[0]; lIndices[0] = lIndices[1]; lIndices[1] = lIndices[2]; lIndices[2] = first;
			CalculateTBN(vertices, intTokens.tokensList, lIndices);
		}

		for (UINT i = 0; i < intTokens.count; ++i)
		{
			free(intTokens.tokensList[i]);
		}

		free(intTokens.tokensList);
		return true;
	};

	while(!in.eof())
	{
		size_t tokensCount = 0;

		std::getline(in, currLine);

		char* cstr = new char[currLine.length() + 1];
		strcpy(cstr, currLine.c_str());

		char* p = std::strtok(cstr, " ");
		while(p != nullptr)
		{
			tokensCount++;
			p = std::strtok(nullptr, " ");
		}

		delete[] cstr;

		Tokens tokens;
		tokens.tokensList = static_cast<char**>(malloc(tokensCount * sizeof(*tokens.tokensList)));

		if(tokens.tokensList)
		{
			cstr = new char[currLine.length() + 1];
			strcpy(cstr, currLine.c_str());

			p = std::strtok(cstr, " ");

			for (UINT i = 0; p != nullptr; ++i)
			{
				tokens.tokensList[i] = static_cast<char*>(malloc(strlen(p) + 1));
				if(tokens.tokensList[i])
				{
					strcpy(tokens.tokensList[i], p);
					p = std::strtok(nullptr, " ");
				}
			}

			delete[] cstr;
		}

		switch(currLine[0])
		{
		case ' ':
		case '#':
		case 's':
		case 'm':
		case 'u':
		case '\n':
		case '\r':
		case '\0':

			for (UINT i = 0; i < tokensCount; ++i)
			{
				free(tokens.tokensList[i]);
			}

			free(tokens.tokensList);
			continue;

		case 'v':
			if(!parseVertex(tokens.tokensList, tokensCount))
			{

				for(UINT i = 0; i < tokensCount; ++i)
				{
					free(tokens.tokensList[i]);
				}

				free(tokens.tokensList);

				return false;
			}
			break;

		case 'f':
			// parse face

			if(!parseFace(tokens.tokensList, tokensCount))
			{
				for (UINT i = 0; i < tokensCount; ++i)
				{
					free(tokens.tokensList[i]);
				}

				free(tokens.tokensList);

				return false;
			}

			break;

		case 'g':
		case 'o':
			if (!buildSubset())
			{
				for (UINT i = 0; i < tokensCount; ++i)
				{
					free(tokens.tokensList[i]);
				}

				free(tokens.tokensList);

				return false;
			}
			break;

		default:

			for (UINT i = 0; i < tokensCount; ++i)
			{
				free(tokens.tokensList[i]);
			}

			free(tokens.tokensList);

			return false;
		}

		for (UINT i = 0; i < tokensCount; ++i)
		{
			free(tokens.tokensList[i]);
		}

		free(tokens.tokensList);
	}

	if (!buildSubset())
		return false;

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(SimpleVertex) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	ZeroMemory(&vInitData, sizeof(vInitData));
	vInitData.pSysMem = &vertices[0];

	auto hr = device->CreateBuffer(&vbd, &vInitData, &modelMesh.vertexBuffer);
	if (FAILED(hr))
		return false;

	_modelCache.insert(std::make_pair(filename, modelMesh));

	return true;
}
