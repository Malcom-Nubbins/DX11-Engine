#include "ModelLoader.h"
#include <iterator>
#include <map>

std::vector<std::string> split(const std::string& text, const std::string& delims)
{
	std::vector<std::string> tokens;
	std::size_t start = text.find_first_not_of(delims), end = 0;

	while((end = text.find_first_of(delims, start)) != std::string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = text.find_first_not_of(delims, end);
	}

	if (start != std::string::npos)
		tokens.push_back(text.substr(start));

	return tokens;
}


void ModelLoader::CalculateTBN(std::vector<SimpleVertex>& vertices, int indices[], int ids[3])
{
	SimpleVertex& v0 = vertices[indices[ids[0]]];
	SimpleVertex& v1 = vertices[indices[ids[1]]];
	SimpleVertex& v2 = vertices[indices[ids[2]]];

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

	auto parseVertex = [&](std::vector<std::string> tokens) -> bool
	{
		if (currLine[1] == 'n')
		{
			// parse normal
			if (!buildSubset())
				return false;

			if (tokens.size() != 4)
				return false;

			norm.x = static_cast<float>(atof(tokens[1].c_str()));
			norm.y = static_cast<float>(atof(tokens[2].c_str()));
			norm.z = static_cast<float>(atof(tokens[3].c_str()));

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

			if (tokens.size() < 3)
				return false;

			texCoord.x = static_cast<float>(atof(tokens[1].c_str()));
			texCoord.y = static_cast<float>(atof(tokens[2].c_str()));

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

			if (tokens.size() != 4)
				return false;

			vert.x = static_cast<float>(atof(tokens[1].c_str()));
			vert.y = static_cast<float>(atof(tokens[2].c_str()));
			vert.z = static_cast<float>(atof(tokens[3].c_str()));

			verts.push_back(vert);
		}
		return true;
	};

	auto parseFace = [&](std::vector<std::string> tokens, 
		std::vector<int> indicesForThisFace,
		std::vector<std::string> faceVals) -> bool
	{
		if (tokens.size() < 4)
		{
			return false;
		}

		indicesForThisFace.reserve(3);

		if(invertFaces)
		{
			for (size_t i = tokens.size() - 1; i > 0; --i)
			{
				const std::string& face = tokens[i];
				if (face.empty())
					continue;

				faceVals.clear();

				faceVals = split(face, "/");

				if (faceVals.size() != 3)
				{
					return false;
				}

				int idPos = -1, idTex = -1, idNorm = -1;

				idPos = static_cast<int>(atoi(faceVals[0].c_str()));
				if (!faceVals[1].empty())
					idTex = static_cast<int>(atoi(faceVals[1].c_str()));

				idNorm = static_cast<int>(atoi(faceVals[2].c_str()));

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
					indicesForThisFace.push_back(vertexIndex);
				}
				else
				{
					indices.push_back(vertexIndex);
					indices.push_back(indicesForThisFace[0]);
					indices.push_back(indicesForThisFace[2]);

					int lIndices[] = { vertexIndex, indicesForThisFace[0], indicesForThisFace[2] };
					int inds[] = { 0, 1, 2 };
					CalculateTBN(vertices, lIndices, inds);
				}
			}
		}
		else
		{
			for (size_t i = 1; i < tokens.size(); ++i)
			{
				const std::string& face = tokens[i];
				if (face.empty())
					continue;

				faceVals.clear();

				faceVals = split(face, "/");

				if (faceVals.size() != 3)
				{
					return false;
				}

				int idPos = -1, idTex = -1, idNorm = -1;

				idPos = static_cast<int>(atoi(faceVals[0].c_str()));
				if (!faceVals[1].empty())
					idTex = static_cast<int>(atoi(faceVals[1].c_str()));

				idNorm = static_cast<int>(atoi(faceVals[2].c_str()));

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
					indicesForThisFace.push_back(vertexIndex);
				}
				else
				{
					indices.push_back(vertexIndex);
					indices.push_back(indicesForThisFace[0]);
					indices.push_back(indicesForThisFace[2]);

					int lIndices[] = { vertexIndex, indicesForThisFace[0], indicesForThisFace[2] };
					int inds[] = { 0, 1, 2 };
					CalculateTBN(vertices, lIndices, inds);
				}

			}
		}

		

		int lIndices[] = { 2, 0, 1 };

		for (int i = 0; i < 3; ++i)
		{
			int first = lIndices[0]; lIndices[0] = lIndices[1]; lIndices[1] = lIndices[2]; lIndices[2] = first;
			CalculateTBN(vertices, &indicesForThisFace[0], lIndices);
		}
		return true;
	};

	while(!in.eof())
	{
		std::getline(in, currLine);
		std::istringstream iss(currLine);
		const std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>{} };

		const std::vector<int> indicesForThisFace;
		const std::vector<std::string> faceVals;

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
			continue;

		case 'v':
			if(!parseVertex(tokens))
			{
				return false;
			}
			break;

		case 'f':
			// parse face
			if(!parseFace(tokens, indicesForThisFace, faceVals))
			{
				return false;
			}
			break;

		case 'g':
		case 'o':
			if (!buildSubset())
				return false;
			break;

		default:

			return false;
		}
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

	return true;
}
