#pragma once
#include "D3DClass.h"
#include "../Core/Loaders/IConfigInterface.h"
#include <vector>

class TextureHandler : public IConfigInterface
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

	struct S_TextureInfo final
	{
		S_TextureInfo(char const* inTextureName, std::wstring const& inTexFilename)
			: m_TextureName(inTextureName)
			, m_TextureFilename(inTexFilename)
		{}
		std::string m_TextureName;
		std::wstring m_TextureFilename;
	};

	std::vector<S_Texture> m_Textures;

public:
	TextureHandler();
	~TextureHandler();
	void Cleanup();

	void LoadAllTextures();
	ID3D11ShaderResourceView* GetTextureByName(char const* name) const;

protected:
	void CreateConfig() override;
	void LoadConfig() override;

private:
	std::vector<S_TextureInfo> m_TextureInfos;
};

