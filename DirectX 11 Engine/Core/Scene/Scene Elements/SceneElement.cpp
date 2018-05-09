#include "SceneElement.h"

SceneElement::SceneElement(std::string elementName, ObjectMesh mesh, ObjectMaterial material) 
	: _mesh(mesh), _material(material), _name(elementName), _position(XMFLOAT3()), _rotation(XMFLOAT3()), _scale(XMFLOAT3(1.0f, 1.0f, 1.0f))
{
	_colourTex = nullptr;
	_normalMap = nullptr;
}

SceneElement::~SceneElement()
{
}

void SceneElement::Update(float deltaTime)
{
	XMMATRIX scale = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	XMMATRIX rotation = XMMatrixRotationX(_rotation.x) * XMMatrixRotationY(_rotation.y) * XMMatrixRotationZ(_rotation.z);
	XMMATRIX translation = XMMatrixTranslation(_position.x, _position.y, _position.z);

	XMStoreFloat4x4(&_world, scale * rotation * translation);
}

void SceneElement::Draw(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetVertexBuffers(0, 1, &_mesh.vertexBuffer, &_mesh.vertexBufferStride, &_mesh.vertexBufferOffset);
	deviceContext->IASetIndexBuffer(_mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(_mesh.numberOfIndices, 0, 0);
}
