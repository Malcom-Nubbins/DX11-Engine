#include "Appearance.h"

#include "../../../ApplicationNew.h"

Appearance::Appearance(ObjectMesh const& mesh, ObjectMaterial const& material)
	: Mesh(mesh), NewMesh(), Material(material), ColourTex(nullptr), NormalMap(nullptr), DisplacementMap(nullptr), SpecularMap(nullptr)
{
}

Appearance::Appearance(NewObjectMesh const& mesh, ObjectMaterial const& material): Mesh(), NewMesh(mesh), Material(material), ColourTex(nullptr),
                                                                     NormalMap(nullptr), DisplacementMap(nullptr),
                                                                     SpecularMap(nullptr)
{
	//NewMesh.IncreaseRefs();
}

Appearance::~Appearance()
{
	if (Mesh.NumberOfIndices > 0)
	{
		Mesh.VertexBuffer.Reset();
		Mesh.IndexBuffer.Reset();
	}

	if (NewMesh.NumOfSubsets > 0)
	{
		NewMesh.VertexBuffer.Reset();

		for (UINT i = 0; i < NewMesh.NumOfSubsets; ++i)
		{
			if (NewMesh.Subsets[i].VertexBuffer)
			{
				NewMesh.Subsets[i].VertexBuffer.Reset();
			}

			if (NewMesh.Subsets[i].IndexBuffer)
			{
				NewMesh.Subsets[i].IndexBuffer.Reset();
			}
		}
	}
}

void Appearance::Cleanup()
{
	if (ColourTex)
	{
		ColourTex->Release();
		ColourTex = nullptr;
	}
	
	if (NormalMap)
	{
		NormalMap->Release();
		NormalMap = nullptr;
	}
	
	if (DisplacementMap)
	{
		DisplacementMap->Release();
		DisplacementMap = nullptr;
	}
	
	if (SpecularMap)
	{
		SpecularMap->Release();
		SpecularMap = nullptr;
	}
}

void Appearance::Draw()
{
	auto context = ApplicationNew::Get().GetContext();

	if(Mesh.NumberOfIndices > 0)
	{
		context->IASetVertexBuffers(0, 1, Mesh.VertexBuffer.GetAddressOf(), &Mesh.vertexBufferStride, &Mesh.VertexBufferOffset);
		context->IASetIndexBuffer(Mesh.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(static_cast<UINT>(Mesh.NumberOfIndices), 0, 0);
	}
	else
	{
		for(UINT i = 0; i < NewMesh.NumOfSubsets; ++i)
		{
			context->IASetVertexBuffers(0, 1, NewMesh.VertexBuffer.GetAddressOf(), &NewMesh.Subsets[i].VertexBufferStride, &NewMesh.Subsets[i].VertexBufferOffset);
			context->IASetIndexBuffer(NewMesh.Subsets[i].IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(NewMesh.Subsets[i].IndexCount, 0, 0);
		}
	}
}
