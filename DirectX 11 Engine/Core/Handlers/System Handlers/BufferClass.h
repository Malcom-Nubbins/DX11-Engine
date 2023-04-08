#pragma once
#include "../../Globals/Structs.h"
#include <vector>

class BufferClass
{
private:

public:
	BufferClass();
	~BufferClass();

	static HRESULT CreateGroundPlane(ID3D11Buffer ** vertexBuffer, ID3D11Buffer ** indexBuffer);
	static HRESULT CreateQuadDynamic(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer);
	static HRESULT CreateQuad(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer);

	void SetIABuffers(ObjectMesh mesh);

	static void SetVertexShaderBuffers(ID3D11Buffer** buffer);
	static void SetVertexShaderBuffers(ID3D11Buffer** buffer, int index);

	static void SetHullShaderBuffers(ID3D11Buffer** buffer);
	static void SetHullShaderBuffers(ID3D11Buffer** buffer, int index);

	static void SetDomainShaderBuffers(ID3D11Buffer** buffer);
	static void SetDomainShaderBuffers(ID3D11Buffer** buffer, int index);

	static void SetPixelShaderBuffers(ID3D11Buffer** constantBuffer);
	static void SetPixelShaderBuffers(ID3D11Buffer** constantBuffer, int index);
};

