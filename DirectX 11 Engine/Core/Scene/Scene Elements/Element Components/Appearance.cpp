#include "Appearance.h"

Appearance::Appearance(ObjectMesh mesh, ObjectMaterial material)
	: _mesh(mesh), _material(material), _colourTex(nullptr), _normalMap(nullptr), _displacementMap(nullptr), _specularMap(nullptr)
{
}

Appearance::~Appearance()
{
}

void Appearance::Draw(ID3D11DeviceContext * context)
{
	//for(int i = 0; i < _mesh.subsets; ++i)
	//{
		context->IASetVertexBuffers(0, 1, &_mesh.vertexBuffer, &_mesh.vertexBufferStride, &_mesh.vertexBufferOffset);
		context->IASetIndexBuffer(_mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//}

	context->DrawIndexed(_mesh.numberOfIndices, 0, 0);
}
