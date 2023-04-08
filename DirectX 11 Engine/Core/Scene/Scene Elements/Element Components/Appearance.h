#pragma once
#include "../../../Globals/stdafx.h"
#include "../../../Globals/Structs.h"
class Appearance
{
private:
	ObjectMesh Mesh;
	NewObjectMesh NewMesh;

	ObjectMaterial Material;

	ID3D11ShaderResourceView* ColourTex;
	ID3D11ShaderResourceView* NormalMap;
	ID3D11ShaderResourceView* DisplacementMap;
	ID3D11ShaderResourceView* SpecularMap;

public:
	Appearance(ObjectMesh const& mesh, ObjectMaterial const& material);
	Appearance(NewObjectMesh const& mesh, ObjectMaterial const& material);
	~Appearance();

	void Cleanup();

	void SetColourTexture(ID3D11ShaderResourceView* colourTex) { ColourTex = colourTex; }
	void SetNormalMap(ID3D11ShaderResourceView* normalMap) { NormalMap = normalMap; }
	void SetDisplacementMap(ID3D11ShaderResourceView* displacementMap) { DisplacementMap = displacementMap; }
	void SetSpecularMap(ID3D11ShaderResourceView* specularMap) { SpecularMap = specularMap; }

	ID3D11ShaderResourceView * GetColourTex() const { return ColourTex; }
	ID3D11ShaderResourceView * GetNormalMap() const { return NormalMap; }
	ID3D11ShaderResourceView * GetDisplacementMap() const { return DisplacementMap; }
	ID3D11ShaderResourceView * GetSpecularMap() const { return SpecularMap; }

	bool HasColourTexture() const { return ColourTex ? true : false; }
	bool HasNormalMap() const { return NormalMap ? true : false; }
	bool HasDisplacementMap() const { return DisplacementMap ? true : false; }
	bool HasSpecularMap() const { return SpecularMap ? true : false; }

	void UpdateObjectMesh(ObjectMesh updatedMesh) { Mesh = updatedMesh; }
	ObjectMesh GetObjectMesh() const { return Mesh; }

	ObjectMaterial GetObjectMaterial() const { return Material; }

	void Draw();

};

