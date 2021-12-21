#include "TextureHandler.h"

#include "../../ApplicationNew.h"
#include "../../Loaders/ConfigLoader.h"
#include <codecvt>
#include <DDSTextureLoader.h>

TextureHandler::TextureHandler() 
{
}

TextureHandler::~TextureHandler()
= default;

void TextureHandler::Cleanup()
{
	for (auto tex : m_Textures)
	{
		if (tex.m_Texture != nullptr)
		{
			tex.m_Texture->Release();
			tex.m_Texture = nullptr;
		}
	}

	m_Textures.clear();
}

void TextureHandler::LoadAllTextures()
{
	ComPtr<ID3D11Device> const& device = ApplicationNew::Get().GetDevice();
	C_ConfigLoader const* const configLoader = ApplicationNew::Get().GetConfigLoader();

	std::vector<S_TextureInfo> const allTextures = configLoader->GetAllTextures();
	assert(!allTextures.empty());

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring texturesPath(converter.from_bytes(configLoader->GetSettingStringValue(SettingType::Engine, "TextureDir")));

	for (auto const& texInfo : allTextures)
	{
		ID3D11ShaderResourceView* tex = nullptr;
		std::wstring textureFilePath(texturesPath + texInfo.m_TextureFilename);

		CreateDDSTextureFromFile(device.Get(), textureFilePath.c_str(), nullptr, &tex);
		if (tex != nullptr)
		{
			S_Texture texture(texInfo.m_TextureName.c_str(), tex);
			m_Textures.emplace_back(std::move(texture));
		}
	}
}

ID3D11ShaderResourceView* TextureHandler::GetTextureByName(char const* name) const
{
	std::string texName(name);

	auto const it = std::find_if(m_Textures.cbegin(), m_Textures.cend(), [texName](S_Texture const& tex)
		{
			return tex.m_Filename == texName;
		});

	if (it != m_Textures.cend())
	{
		(*it).m_Texture->AddRef();
		return (*it).m_Texture;
	}
	else
	{
		std::string defaultTexName("Default");
		auto const defaultIt = std::find_if(m_Textures.cbegin(), m_Textures.cend(), [defaultTexName](S_Texture const& tex)
			{
				return tex.m_Filename == defaultTexName;
			});

		if (defaultIt != m_Textures.cend())
		{
			(*defaultIt).m_Texture->AddRef();
			return (*defaultIt).m_Texture;
		}
	}

	return nullptr;
}
