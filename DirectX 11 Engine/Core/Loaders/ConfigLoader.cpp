#include "ConfigLoader.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <codecvt>

#include "../Core/Loaders/XMLLoader/rapidxml.hpp"
#include "../Loaders/XMLLoader/rapidxml_ext.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_print.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_utils.hpp"

C_ConfigLoader::C_ConfigLoader(std::string const& inFilename)
	: m_ConfigFilename(inFilename)
{
}

C_ConfigLoader::~C_ConfigLoader()
{
}

bool C_ConfigLoader::CheckConfigExists(std::string const& inConfigFilename)
{
	struct stat buffer;
	return (stat(inConfigFilename.c_str(), &buffer) == 0);
}

void C_ConfigLoader::CreateDefaultMainConfig()
{
	using namespace std;
	using namespace rapidxml;

	ofstream newMainConfig(m_ConfigFilename);

	xml_document<> doc;
	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
	doc.append_node(decl);

	xml_node<>* root = doc.allocate_node(node_element, "Settings");
	doc.append_node(root);

	newMainConfig << doc;

	newMainConfig.close();
	doc.clear();
}

void C_ConfigLoader::CreateDefaultConfigFile(std::string const& inConfigFilename)
{
}

void C_ConfigLoader::Initialise()
{
	using namespace std;
	using namespace rapidxml;

	/*if (!CheckConfigExists(m_ConfigFilename))
	{
		CreateDefaultMainConfig();
	}*/

	file<> xmlFile(m_ConfigFilename.c_str());
	xml_document<> doc;

	doc.parse<0>(xmlFile.data());

	xml_node<>* rootNode = doc.first_node("Settings");

	for (xml_node<>* settingsNode = rootNode->first_node("Setting"); settingsNode; settingsNode = settingsNode->next_sibling())
	{
		for (xml_node<>* engineNode = settingsNode->first_node("EngineSetting"); engineNode; engineNode = engineNode->next_sibling())
		{
			string nodeValue(engineNode->value());

			S_SettingNameValuePair engineSetting(SettingType::Engine, 
				engineNode->first_attribute("name")->value(), 
				-1, 
				nodeValue.c_str());

			m_AllSettings.emplace_back(move(engineSetting));
		}

		for (xml_node<>* graphicsNode = settingsNode->first_node("Graphics"); graphicsNode; graphicsNode = graphicsNode->next_sibling())
		{
			int const nodeValue = stoi(string(graphicsNode->value()));

			S_SettingNameValuePair graphicsSetting(SettingType::Graphics,
				graphicsNode->first_attribute("name")->value(),
				nodeValue,
				"");

			m_AllSettings.emplace_back(move(graphicsSetting));
		}

		for (xml_node<>* videoNode = settingsNode->first_node("VideoSetting"); videoNode; videoNode = videoNode->next_sibling())
		{
			int const nodeValue = stoi(string(videoNode->value()));

			S_SettingNameValuePair videoSetting(SettingType::Video,
				videoNode->first_attribute("name")->value(),
				nodeValue, 
				"");

			m_AllSettings.emplace_back(move(videoSetting));
		}
	}	

	InitConfigsList();
}

int C_ConfigLoader::GetSettingValue(SettingType const settingType, char const* settingName) const
{
	auto const it = std::find_if(m_AllSettings.cbegin(), m_AllSettings.cend(), [settingType, settingName](S_SettingNameValuePair const& pair)
		{
			std::string settingNameStr(settingName);
			return (pair.m_Type == settingType) && (settingNameStr == pair.m_SettingName);
		});

	if (it != m_AllSettings.cend())
	{
		return (*it).m_SettingValue;
	}

	return -1;
}

std::string C_ConfigLoader::GetSettingStringValue(SettingType const settingType, char const* settingName) const
{
	auto const it = std::find_if(m_AllSettings.cbegin(), m_AllSettings.cend(), [settingType, settingName](S_SettingNameValuePair const& pair)
		{
			std::string settingNameStr(settingName);
			return (pair.m_Type == settingType) && (settingNameStr == pair.m_SettingName);
		});

	if (it != m_AllSettings.cend())
	{
		return (*it).m_SettingValueString;
	}

	return std::string("");
}

std::vector<S_TextureInfo> C_ConfigLoader::GetAllTextures() const
{
	using namespace rapidxml;
	using namespace std;

	string textureListFilename(GetSettingStringValue(SettingType::Engine, "TexturesListFile"));
	string filePath(GetSettingStringValue(SettingType::Engine, "TextureDir"));

	string fullPath(filePath + textureListFilename);

	file<> xmlFile(fullPath.c_str());
	xml_document<> doc;

	doc.parse<0>(xmlFile.data());

	xml_node<>* rootNode = doc.first_node("Textures");

	vector<S_TextureInfo> allTextures;

	for (xml_node<>* textureNode = rootNode->first_node("Texture"); textureNode; textureNode = textureNode->next_sibling())
	{
		wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		wstring nodeValue(converter.from_bytes(textureNode->value()));

		S_TextureInfo texInfo(textureNode->first_attribute("name")->value(), nodeValue);

		allTextures.emplace_back(move(texInfo));
	}

	return allTextures;
}

S_ConfigInfo C_ConfigLoader::GetConfigByName(char const* const inConfigName) const
{
	auto const it = std::find_if(m_AllConfigs.cbegin(), m_AllConfigs.cend(), [inConfigName](S_ConfigInfo const& configInfo)
		{
			std::string const configNameStr(inConfigName);
			return configNameStr == configInfo.m_ConfigName;
		});

	if (it != m_AllConfigs.cend())
	{
		return (*it);
	}

	return S_ConfigInfo();
}

void C_ConfigLoader::ReloadAllConfigs()
{
	m_AllConfigs.clear();
	InitConfigsList();
}

void C_ConfigLoader::InitConfigsList()
{
	using namespace rapidxml;
	using namespace std;

	string configListFilename(GetSettingStringValue(SettingType::Engine, "ConfigListFile"));
	string filePath(GetSettingStringValue(SettingType::Engine, "ConfigDir"));

	string fullPath(filePath + configListFilename);

	file<> xmlFile(fullPath.c_str());
	xml_document<> doc;

	doc.parse<0>(xmlFile.data());

	xml_node<>* rootNode = doc.first_node("Configs");

	for (xml_node<>* configNode = rootNode->first_node("Config"); configNode; configNode = configNode->next_sibling())
	{
		wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		wstring nodeValue(converter.from_bytes(configNode->value()));

		S_ConfigInfo configInfo(configNode->first_attribute("name")->value(), nodeValue);

		m_AllConfigs.emplace_back(move(configInfo));
	}
}
