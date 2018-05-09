#pragma once
#include "../../Globals/stdafx.h"
#include "../../Globals/Structs.h"
#include <string>

using namespace DirectX;

class SceneElement
{
private:
	XMFLOAT3 _position;
	XMFLOAT3 _rotation;
	XMFLOAT3 _scale;

	ObjectMesh _mesh;
	ObjectMaterial _material;

	std::string _name;

	XMFLOAT4X4 _world;

	ID3D11ShaderResourceView* _colourTex;
	ID3D11ShaderResourceView* _normalMap;
	ID3D11ShaderResourceView* _displacementMap;

	bool _castShadows;
	bool _affectedByLight;

public:
	SceneElement(std::string elementName, ObjectMesh mesh, ObjectMaterial material);
	~SceneElement();

	void Update(float deltaTime);
	void Draw(ID3D11DeviceContext* deviceContext);

public:
	void SetPosition(XMFLOAT3 pos) { _position = pos; }
	XMFLOAT3 GetPosition() const { return _position; }

	void SetRotation(XMFLOAT3 rot) { _rotation = rot; }
	XMFLOAT3 GetRotation() const { return _rotation; }

	void SetScale(XMFLOAT3 scale) { _scale = scale; }
	XMFLOAT3 GetScale() const { return _scale; }

	void SetColourTexture(ID3D11ShaderResourceView* colourTex) { _colourTex = colourTex; }
	void SetNormalMap(ID3D11ShaderResourceView* normalMap) { _normalMap = normalMap; }
	void SetDisplacementMap(ID3D11ShaderResourceView* displacementMap) { _displacementMap = displacementMap; }

	void SetCastShadows(bool castShadows) { _castShadows = castShadows; }
	bool CanCastShadows() const { return _castShadows; }

	void SetAffectedByLight(bool affected) { _affectedByLight = affected; }
	bool IsAffectedByLight() const { return _affectedByLight; }

	ID3D11ShaderResourceView * GetColourTex() const { return _colourTex; }
	ID3D11ShaderResourceView * GetNormalMap() const { return _normalMap; }
	ID3D11ShaderResourceView * GetDisplacementMap() const { return _displacementMap; }

	bool HasColourTexture() const { return _colourTex ? true : false; }
	bool HasNormalMap() const { return _normalMap ? true : false; }
	bool HasDisplacementMap() const { return _displacementMap ? true : false; }

	void UpdateObjectMesh(ObjectMesh updatedMesh) { _mesh = updatedMesh; }
	ObjectMesh GetObjectMesh() const { return _mesh; }

	ObjectMaterial GetObjectMaterial() const { return _material; }
	XMFLOAT4X4 GetWorld() const { return _world; }

	std::string GetElementName() const { return _name; }
};

