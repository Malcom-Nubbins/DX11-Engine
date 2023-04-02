#include "TextureHandler.h"

#include "../../ApplicationNew.h"
#include "../../Loaders/ConfigLoader.h"
#include <codecvt>
#include <DDSTextureLoader.h>

TextureHandler::TextureHandler() 
	: IConfigInterface(TEXTURES_CONFIG)
{
	LoadConfig();
}

TextureHandler::~TextureHandler()
{
}

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

	//assert(!m_TextureInfos.empty());

	if (m_TextureInfos.empty())
	{
		return;
	}

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring texturesPath(converter.from_bytes(configLoader->GetSettingStringValue(SettingType::Engine, "TextureDir")));

	for (auto const& texInfo : m_TextureInfos)
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

void TextureHandler::CreateConfig()
{
	using namespace std;
	using namespace rapidxml;

	ofstream newTexturesListConfig(TEXTURES_CONFIG);

	xml_document<> doc;
	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
	doc.append_node(decl);

	xml_node<>* root = doc.allocate_node(node_element, "Textures");
	doc.append_node(root);

	xml_node<>* defaultTex = doc.allocate_node(node_element, "Texture");
	defaultTex->append_attribute(doc.allocate_attribute("name", "Default"));
	defaultTex->value("Default.dds");
	root->append_node(defaultTex);

	newTexturesListConfig << doc;

	newTexturesListConfig.close();
	doc.clear();
}

void TextureHandler::LoadConfig()
{
	using namespace rapidxml;
	using namespace std;

	if (!DoesConfigExist())
	{
		CreateConfig();
	}

	file<> xmlFile(TEXTURES_CONFIG);
	xml_document<> doc;

	doc.parse<0>(xmlFile.data());

	xml_node<>* rootNode = doc.first_node("Textures");

	for (xml_node<>* textureNode = rootNode->first_node("Texture"); textureNode; textureNode = textureNode->next_sibling())
	{
		wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		wstring nodeValue(converter.from_bytes(textureNode->value()));

		S_TextureInfo texInfo(textureNode->first_attribute("name")->value(), nodeValue);

		m_TextureInfos.emplace_back(move(texInfo));
	}
}
