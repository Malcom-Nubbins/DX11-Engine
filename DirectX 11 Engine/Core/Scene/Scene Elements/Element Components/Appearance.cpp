#include "Appearance.h"

#include "../../../ApplicationNew.h"

Appearance::Appearance(ObjectMesh mesh, ObjectMaterial material)
	: _mesh(mesh), _newMesh(), _material(material), _colourTex(nullptr), _normalMap(nullptr), _displacementMap(nullptr), _specularMap(nullptr)
{
}

Appearance::Appearance(NewObjectMesh mesh, ObjectMaterial material): _mesh(), _newMesh(mesh), _material(material), _colourTex(nullptr),
                                                                     _normalMap(nullptr), _displacementMap(nullptr),
                                                                     _specularMap(nullptr)
{
	_newMesh.IncreaseRefs();
}

Appearance::~Appearance()
{
}

void Appearance::Cleanup()
{
	if (_colourTex)
	{
		_colourTex->Release();
		_colourTex = nullptr;
	}
	
	if (_normalMap)
	{
		_normalMap->Release();
		_colourTex = nullptr;
	}
	
	if (_displacementMap)
	{
		_displacementMap->Release();
		_colourTex = nullptr;
	}
	
	if (_specularMap)
	{
		_specularMap->Release();
		_colourTex = nullptr;
	}
	
	if (_mesh.numberOfIndices > 0)
	{
		if (_mesh.vertexBuffer)
		{
			_mesh.vertexBuffer->Release();
		}
		
		if (_mesh.indexBuffer)
		{
			_mesh.indexBuffer->Release();
		}
	}
	
	if (_newMesh.numOfSubsets > 0)
	{
		_newMesh.vertexBuffer->Release();
		for (UINT i = 0; i < _newMesh.numOfSubsets; ++i)
		{
			if (_newMesh.subsets[i].vertexBuffer)
			{
				_newMesh.subsets[i].vertexBuffer->Release();
			}
			
			if (_newMesh.subsets[i].indexBuffer)
			{
				_newMesh.subsets[i].indexBuffer->Release();
			}
		}
	}
}

void Appearance::Draw()
{
	auto context = ApplicationNew::Get().GetContext();

	if(_mesh.numberOfIndices > 0)
	{
		context->IASetVertexBuffers(0, 1, &_mesh.vertexBuffer, &_mesh.vertexBufferStride, &_mesh.vertexBufferOffset);
		context->IASetIndexBuffer(_mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(static_cast<UINT>(_mesh.numberOfIndices), 0, 0);
	}
	else
	{
		for(UINT i = 0; i < _newMesh.numOfSubsets; ++i)
		{
			context->IASetVertexBuffers(0, 1, &_newMesh.vertexBuffer, &_newMesh.subsets[i].vertexBufferStride, &_newMesh.subsets[i].vertexBufferOffset);
			context->IASetIndexBuffer(_newMesh.subsets[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(_newMesh.subsets[i].indexCount, 0, 0);
		}
	}
}
