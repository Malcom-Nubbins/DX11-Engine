#pragma once
#include "../../../Globals/stdafx.h"
#include "../../../Globals/Structs.h"
class Appearance
{
private:
	ObjectMesh _mesh;
	ObjectMaterial _material;

	ID3D11ShaderResourceView* _colourTex;
	ID3D11ShaderResourceView* _normalMap;
	ID3D11ShaderResourceView* _displacementMap;

public:
	Appearance(ObjectMesh mesh, ObjectMaterial material);
	~Appearance();

	void SetColourTexture(ID3D11ShaderResourceView* colourTex) { _colourTex = colourTex; }
	void SetNormalMap(ID3D11ShaderResourceView* normalMap) { _normalMap = normalMap; }
	void SetDisplacementMap(ID3D11ShaderResourceView* displacementMap) { _displacementMap = displacementMap; }

	ID3D11ShaderResourceView * GetColourTex() const { return _colourTex; }
	ID3D11ShaderResourceView * GetNormalMap() const { return _normalMap; }
	ID3D11ShaderResourceView * GetDisplacementMap() const { return _displacementMap; }

	bool HasColourTexture() const { return _colourTex ? true : false; }
	bool HasNormalMap() const { return _normalMap ? true : false; }
	bool HasDisplacementMap() const { return _displacementMap ? true : false; }

	void UpdateObjectMesh(ObjectMesh updatedMesh) { _mesh = updatedMesh; }
	ObjectMesh GetObjectMesh() const { return _mesh; }

	ObjectMaterial GetObjectMaterial() const { return _material; }

	void Draw(ID3D11DeviceContext* context);

};

