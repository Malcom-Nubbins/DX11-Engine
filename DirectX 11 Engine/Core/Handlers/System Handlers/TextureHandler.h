#pragma once
#include "../Core/Loaders/IConfigInterface.h"
#include <vector>

class TextureHandler : public IConfigInterface
{
private:
	struct TextureDesc final
	{
		TextureDesc(char const* inName, ID3D11ShaderResourceView* inTex)
			: Filename(inName), Texture(inTex)
		{}

		std::string Filename;
		ID3D11ShaderResourceView* Texture;
	};

	struct TextureInfo final
	{
		TextureInfo(char const* inTextureName, std::wstring const& inTexFilename)
			: TextureName(inTextureName)
			, TextureFilename(inTexFilename)
		{}
		std::string TextureName;
		std::wstring TextureFilename;
	};

	std::vector<TextureDesc> m_Textures;

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
	std::vector<TextureInfo> m_TextureInfos;
};

