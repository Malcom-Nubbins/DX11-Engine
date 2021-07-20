#pragma once
#include "D3DClass.h"
#include "../../Loaders/DDSTextureLoader.h"
#include <vector>

class TextureHandler
{
private:
	struct S_Texture final
	{
		S_Texture(char const* inName, ID3D11ShaderResourceView* inTex)
			: m_Filename(inName), m_Texture(inTex)
		{}

		std::string m_Filename;
		ID3D11ShaderResourceView* m_Texture;
	};

	std::vector<S_Texture> m_Textures;

public:
	TextureHandler();
	~TextureHandler();
	void Cleanup();

	void LoadAllTextures();
	ID3D11ShaderResourceView* GetTextureByName(char const* name) const;
};

