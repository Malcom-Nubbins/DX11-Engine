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

	ID3D11ShaderResourceView* _stoneTex;
	ID3D11ShaderResourceView* _stoneBumpMap;
	ID3D11ShaderResourceView* _stoneDisplacementMap;

	ID3D11ShaderResourceView* _distortionMap;
	ID3D11ShaderResourceView* _snowTexture;
public:
	TextureHandler(D3DClass* d3dClass);
	~TextureHandler();
	void Cleanup();

	void LoadAllTextures();

	ID3D11ShaderResourceView* GetGroundColourTexture() const { return _groundColourTex; }
	ID3D11ShaderResourceView* GetGroundNormalMap() const { return _groundNormalMap; }
	ID3D11ShaderResourceView* GetGroundDisplacementMap() const { return _groundDisplacementMap; }

	ID3D11ShaderResourceView* GetStoneTexture() const { return _stoneTex; }
	ID3D11ShaderResourceView* GetStoneNormalMap() const { return _stoneBumpMap; }
	ID3D11ShaderResourceView* GetStoneDisplacementMap() const { return _stoneDisplacementMap; }

	ID3D11ShaderResourceView* GetDistortionMap() const { return _distortionMap; }
	ID3D11ShaderResourceView* GetSnowTexture() const { return _snowTexture; }
};

