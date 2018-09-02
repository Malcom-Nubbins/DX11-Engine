#include "Appearance.h"

Appearance::Appearance(ObjectMesh mesh, ObjectMaterial material)
	: _mesh(mesh), _material(material), _colourTex(nullptr), _normalMap(nullptr), _displacementMap(nullptr), _specularMap(nullptr)
{
}

Appearance::Appearance(NewObjectMesh mesh, ObjectMaterial material): _newMesh(mesh), _material(material), _colourTex(nullptr),
                                                                     _normalMap(nullptr), _displacementMap(nullptr),
                                                                     _specularMap(nullptr)
{
}

Appearance::~Appearance()
{
}

void Appearance::Draw(ID3D11DeviceContext * context)
{
	if(_mesh.numberOfIndices != -1)
	{
		context->IASetVertexBuffers(0, 1, &_mesh.vertexBuffer, &_mesh.vertexBufferStride, &_mesh.vertexBufferOffset);
		context->IASetIndexBuffer(_mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(_mesh.numberOfIndices, 0, 0);
	}
	else
	{
		for(int i = 0; i < _newMesh.numOfSubsets; ++i)
		{
			context->IASetVertexBuffers(0, 1, &_newMesh.vertexBuffer, &_newMesh.subsets[i].vertexBufferStride, &_newMesh.subsets[i].vertexBufferOffset);
			context->IASetIndexBuffer(_newMesh.subsets[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(_newMesh.subsets[i].indexCount, 0, 0);
		}
	}
}
