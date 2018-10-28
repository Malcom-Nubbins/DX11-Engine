#pragma once
#include "../../../Handlers/D3DClass.h"
#include "../../../Loaders/DDSTextureLoader.h"

class TextureHandler
{
private:
	D3DClass * _d3dClass;

	ID3D11ShaderResourceView* _groundColourTex;
	ID3D11ShaderResourceView* _groundNormalMap;
	ID3D11ShaderResourceView* _groundDisplacementMap;
	ID3D11ShaderResourceView* _groundSpecularMap;

	ID3D11ShaderResourceView* _stoneTex;
	ID3D11ShaderResourceView* _stoneBumpMap;
	ID3D11ShaderResourceView* _stoneDisplacementMap;
	ID3D11ShaderResourceView* _stoneSpecularMap;

	ID3D11ShaderResourceView* _distortionMap;
	ID3D11ShaderResourceView* _snowTexture;

	ID3D11ShaderResourceView* _aircraftTexture;
	ID3D11ShaderResourceView* _aircraftNormalMap;
	ID3D11ShaderResourceView* _aircraftSpecularMap;
	ID3D11ShaderResourceView* _aircraftDisplacementMap;

	ID3D11ShaderResourceView* _metalFloorColourTex;
	ID3D11ShaderResourceView* _metalFloorNormalMap;

public:
	TextureHandler(D3DClass* d3dClass);
	~TextureHandler();
	void Cleanup();

	void LoadAllTextures();

	ID3D11ShaderResourceView* GetGroundColourTexture() const { return _groundColourTex; }
	ID3D11ShaderResourceView* GetGroundNormalMap() const { return _groundNormalMap; }
	ID3D11ShaderResourceView* GetGroundDisplacementMap() const { return _groundDisplacementMap; }
	ID3D11ShaderResourceView* GetGroundSpecularMap() const { return _groundSpecularMap; }

	ID3D11ShaderResourceView* GetStoneTexture() const { return _stoneTex; }
	ID3D11ShaderResourceView* GetStoneNormalMap() const { return _stoneBumpMap; }
	ID3D11ShaderResourceView* GetStoneDisplacementMap() const { return _stoneDisplacementMap; }
	ID3D11ShaderResourceView* GetStoneSpecularMap() const { return _stoneSpecularMap; }

	ID3D11ShaderResourceView* GetDistortionMap() const { return _distortionMap; }
	ID3D11ShaderResourceView* GetSnowTexture() const { return _snowTexture; }

	ID3D11ShaderResourceView* GetAircraftTexture() const { return _aircraftTexture; }
	ID3D11ShaderResourceView* GetAircraftNormalMap() const { return _aircraftNormalMap; }
	ID3D11ShaderResourceView* GetAircraftSpecularMap() const { return _aircraftSpecularMap; }
	ID3D11ShaderResourceView* GetAircraftDisplacementMap() const { return _aircraftDisplacementMap; }

	ID3D11ShaderResourceView* GetMetalFloorColourTexture() const { return _metalFloorColourTex; }
	ID3D11ShaderResourceView* GetMetalFloorNormalMap() const { return _metalFloorNormalMap; }
};

