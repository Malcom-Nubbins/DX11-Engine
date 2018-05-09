#pragma once
#include "D3DClass.h"
#include "../Globals/Structs.h"
#include <vector>
class BufferClass
{
private:
	D3DClass * _d3dClass;

public:
	BufferClass(D3DClass* d3dClass);
	~BufferClass();

	HRESULT CreateGroundPlane(ID3D11Buffer ** vertexBuffer, ID3D11Buffer ** indexBuffer);
	HRESULT CreateQuad(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer);

	void SetIABuffers(ObjectMesh mesh);

	void SetVertexShaderBuffers(ID3D11Buffer** buffer);
	void SetVertexShaderBuffers(ID3D11Buffer** buffer, int index);

	void SetHullShaderBuffers(ID3D11Buffer** buffer);
	void SetHullShaderBuffers(ID3D11Buffer** buffer, int index);

	void SetDomainShaderBuffers(ID3D11Buffer** buffer);
	void SetDomainShaderBuffers(ID3D11Buffer** buffer, int index);

	void SetPixelShaderBuffers(ID3D11Buffer** constantBuffer);
	void SetPixelShaderBuffers(ID3D11Buffer** constantBuffer, int index);
};

