#pragma once
#include "D3DClass.h"
#include "../../Loaders/DDSTextureLoader.h"
#include <map>

class TextureHandler
{
private:
	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_Context;

	static std::map<char const*, ID3D11ShaderResourceView*> m_Textures;

	ID3D11ShaderResourceView* m_GroundColourTex;
	ID3D11ShaderResourceView* m_GroundNormalMap;
	ID3D11ShaderResourceView* m_GroundDisplacementMap;
	ID3D11ShaderResourceView* m_GroundSpecularMap;

	ID3D11ShaderResourceView* m_stoneTex;
	ID3D11ShaderResourceView* m_stoneBumpMap;
	ID3D11ShaderResourceView* m_stoneDisplacementMap;
	ID3D11ShaderResourceView* m_stoneSpecularMap;

	ID3D11ShaderResourceView* m_distortionMap;
	ID3D11ShaderResourceView* m_snowTexture;

	ID3D11ShaderResourceView* m_aircraftTexture;
	ID3D11ShaderResourceView* m_aircraftNormalMap;
	ID3D11ShaderResourceView* m_aircraftSpecularMap;
	ID3D11ShaderResourceView* m_aircraftDisplacementMap;

	ID3D11ShaderResourceView* m_metalFloorColourTex;
	ID3D11ShaderResourceView* m_metalFloorNormalMap;

public:
	TextureHandler(ID3D11Device* device, ID3D11DeviceContext* context);
	~TextureHandler();
	void Cleanup();

	void LoadAllTextures();
	static ID3D11ShaderResourceView* GetTextureByName(char const* name);
};

