#include "ConfigLoader.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <codecvt>

C_ConfigLoader::C_ConfigLoader(std::string const& inFilename)
	: m_ConfigFilename(inFilename)
{
}

C_ConfigLoader::~C_ConfigLoader()
{
}

void C_ConfigLoader::Initialise()
{
	using namespace std;
	using namespace rapidxml;

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
