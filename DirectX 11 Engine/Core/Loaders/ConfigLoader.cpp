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

bool C_ConfigLoader::CheckConfigExists(std::string const& inConfigFilename) const
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

	xml_node<>* settingEngine = doc.allocate_node(node_element, "Setting");
	settingEngine->append_attribute(doc.allocate_attribute("name", "Engine"));
	root->append_node(settingEngine);

	// Engine configs defaults
	{
		xml_node<>* configDir = doc.allocate_node(node_element, "EngineSetting");
		configDir->append_attribute(doc.allocate_attribute("name", "ConfigDir"));
		configDir->append_attribute(doc.allocate_attribute("description", ""));
		configDir->value("Core/Config/");
		settingEngine->append_node(configDir);

		xml_node<>* textureDir = doc.allocate_node(node_element, "EngineSetting");
		textureDir->append_attribute(doc.allocate_attribute("name", "TextureDir"));
		textureDir->append_attribute(doc.allocate_attribute("description", "Path to where textures are kept"));
		textureDir->value("Core/Resources/Textures/");
		settingEngine->append_node(textureDir);

		xml_node<>* texturesListFile = doc.allocate_node(node_element, "EngineSetting");
		texturesListFile->append_attribute(doc.allocate_attribute("name", "TexturesListFile"));
		texturesListFile->append_attribute(doc.allocate_attribute("description", "Filename of the file containing name/filename pairs for all textures"));
		texturesListFile->value("_textures_list.xml");
		settingEngine->append_node(texturesListFile);

		xml_node<>* materialsListFile = doc.allocate_node(node_element, "EngineSetting");
		materialsListFile->append_attribute(doc.allocate_attribute("name", "MaterialsListFile"));
		materialsListFile->append_attribute(doc.allocate_attribute("description", "Filename of the file containing info on each material"));
		materialsListFile->value("_materials.xml");
		settingEngine->append_node(materialsListFile);

		xml_node<>* configsListFile = doc.allocate_node(node_element, "EngineSetting");
		configsListFile->append_attribute(doc.allocate_attribute("name", "ConfigListFile"));
		configsListFile->append_attribute(doc.allocate_attribute("description", "Filename of the file containing name/filename pairs for config files"));
		configsListFile->value("_configs_list.xml");
		settingEngine->append_node(configsListFile);
	}

	xml_node<>* settingGraphics = doc.allocate_node(node_element, "Setting");
	settingGraphics->append_attribute(doc.allocate_attribute("name", "Graphics Settings"));
	root->append_node(settingGraphics);

	// Graphics configs defaults
	{
		xml_node<>* msaa = doc.allocate_node(node_element, "Graphics");
		msaa->append_attribute(doc.allocate_attribute("name", "MSAA"));
		msaa->append_attribute(doc.allocate_attribute("description", "Multisampled Antialiasing quality. Must be power of 2, up to 8 and no less than 1"));
		msaa->value("1");
		settingGraphics->append_node(msaa);

		xml_node<>* shadows = doc.allocate_node(node_element, "Graphics");
		shadows->append_attribute(doc.allocate_attribute("name", "Shadows"));
		shadows->append_attribute(doc.allocate_attribute("description", "Controls the quality of directional shadows"));
		shadows->value("1024");
		settingGraphics->append_node(shadows);
	}

	xml_node<>* settingsVideo = doc.allocate_node(node_element, "Setting");
	settingsVideo->append_attribute(doc.allocate_attribute("name", "Video"));
	root->append_node(settingsVideo);

	// Video configs defaults
	{
		xml_node<>* screenWidth = doc.allocate_node(node_element, "VideoSetting");
		screenWidth->append_attribute(doc.allocate_attribute("name", "ScreenWidth"));
		screenWidth->append_attribute(doc.allocate_attribute("description", "Horizontal Resolution"));
		screenWidth->value("1600");
		settingsVideo->append_node(screenWidth);

		xml_node<>* screenHeight = doc.allocate_node(node_element, "VideoSetting");
		screenHeight->append_attribute(doc.allocate_attribute("name", "ScreenHeight"));
		screenHeight->append_attribute(doc.allocate_attribute("description", "Vertical Resolution"));
		screenHeight->value("900");
		settingsVideo->append_node(screenHeight);

		xml_node<>* vsync = doc.allocate_node(node_element, "VideoSetting");
		vsync->append_attribute(doc.allocate_attribute("name", "VSync"));
		vsync->append_attribute(doc.allocate_attribute("description", "Sync refresh rate to monitor. Set to 0 for no, 1 for yes"));
		vsync->value("1");
		settingsVideo->append_node(vsync);
	}

	newMainConfig << doc;

	newMainConfig.close();
	doc.clear();
}

void C_ConfigLoader::CreateDefaultTexturesConfig(std::string const& inConfigFilename) const
{
	using namespace std;
	using namespace rapidxml;

	ofstream newTexturesListConfig(inConfigFilename);

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

void C_ConfigLoader::CreateDefaultConfigsListConfig(std::string const& inConfigFilename) const
{
	using namespace std;
	using namespace rapidxml;

	ofstream newConfigsList(inConfigFilename);

	xml_document<> doc;
	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
	doc.append_node(decl);

	xml_node<>* root = doc.allocate_node(node_element, "Configs");
	doc.append_node(root);

	xml_node<>* uiConfig = doc.allocate_node(node_element, "Config");
	uiConfig->append_attribute(doc.allocate_attribute("name", "UIConfig"));
	uiConfig->append_attribute(doc.allocate_attribute("description", "Config file for controlling UI layout"));
	uiConfig->value("ui_config.xml");
	root->append_node(uiConfig);

	xml_node<>* scenesConfig = doc.allocate_node(node_element, "Config");
	scenesConfig->append_attribute(doc.allocate_attribute("name", "ScenesConfig"));
	scenesConfig->append_attribute(doc.allocate_attribute("description", "Config file containing all game scenes"));
	scenesConfig->value("scenes_config.xml");
	root->append_node(scenesConfig);

	newConfigsList << doc;

	newConfigsList.close();
	doc.clear();
}

void C_ConfigLoader::CreateDefaultMaterialsConfig(std::string const& inConfigFilename) const
{
	using namespace std;
	using namespace rapidxml;

	ofstream newMaterialsListConfig(inConfigFilename);

	xml_document<> doc;
	xml_node<>* decl = doc.allocate_node(node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "UTF-8"));
	doc.append_node(decl);

	xml_node<>* root = doc.allocate_node(node_element, "Materials");
	doc.append_node(root);

	xml_node<>* defaultMat = doc.allocate_node(node_element, "Material");
	defaultMat->append_attribute(doc.allocate_attribute("name", "Default"));
	root->append_node(defaultMat);

	{
		xml_node<>* ambient = doc.allocate_node(node_element, "Ambient");
		ambient->append_attribute(doc.allocate_attribute("red", "1.0"));
		ambient->append_attribute(doc.allocate_attribute("green", "1.0"));
		ambient->append_attribute(doc.allocate_attribute("blue", "1.0"));
		
		defaultMat->append_node(ambient);

		xml_node<>* diffuse = doc.allocate_node(node_element, "Diffuse");
		diffuse->append_attribute(doc.allocate_attribute("red", "1.0"));
		diffuse->append_attribute(doc.allocate_attribute("green", "1.0"));
		diffuse->append_attribute(doc.allocate_attribute("blue", "1.0"));

		defaultMat->append_node(diffuse);

		xml_node<>* specular = doc.allocate_node(node_element, "Specular");
		specular->append_attribute(doc.allocate_attribute("red", "1.0"));
		specular->append_attribute(doc.allocate_attribute("green", "1.0"));
		specular->append_attribute(doc.allocate_attribute("blue", "1.0"));
		specular->append_attribute(doc.allocate_attribute("shinyness", "0.5"));

		defaultMat->append_node(specular);
	}

	newMaterialsListConfig << doc;

	newMaterialsListConfig.close();
	doc.clear();
}

void C_ConfigLoader::Initialise()
{
	using namespace std;
	using namespace rapidxml;

	if (!CheckConfigExists(m_ConfigFilename))
	{
		CreateDefaultMainConfig();
	}

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

	if (!CheckConfigExists(fullPath))
	{
		CreateDefaultTexturesConfig(fullPath);
	}

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

std::vector<S_MaterialInfo> C_ConfigLoader::GetAllMaterials() const
{
	using namespace rapidxml;
	using namespace std;

	string materialsListFilename(GetSettingStringValue(SettingType::Engine, "MaterialsListFile"));
	string filePath(GetSettingStringValue(SettingType::Engine, "TextureDir"));

	string fullPath(filePath + materialsListFilename);

	if (!CheckConfigExists(fullPath))
	{
		CreateDefaultMaterialsConfig(fullPath);
	}

	file<> xmlFile(fullPath.c_str());
	xml_document<> doc;

	doc.parse<0>(xmlFile.data());

	xml_node<>* rootNode = doc.first_node("Materials");

	std::vector<S_MaterialInfo> allMaterials;

	for (xml_node<>* materialNode = rootNode->first_node("Material"); materialNode; materialNode = materialNode->next_sibling())
	{
		wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		wstring nodeValue(converter.from_bytes(materialNode->value()));

		string const materialName(materialNode->first_attribute("name")->value());
		
		xml_node<>* ambientNode = materialNode->first_node("Ambient");
		xml_node<>* diffuseNode = materialNode->first_node("Diffuse");
		xml_node<>* specularNode = materialNode->first_node("Specular");

		XMFLOAT4 ambient(strtof(ambientNode->first_attribute("red")->value(), nullptr), 
			strtof(ambientNode->first_attribute("green")->value(), nullptr),
			strtof(ambientNode->first_attribute("blue")->value(), nullptr), 1.0f);

		XMFLOAT4 diffuse(strtof(diffuseNode->first_attribute("red")->value(), nullptr),
			strtof(diffuseNode->first_attribute("green")->value(), nullptr),
			strtof(diffuseNode->first_attribute("blue")->value(), nullptr), 1.0f);

		XMFLOAT4 specular(strtof(specularNode->first_attribute("red")->value(), nullptr),
			strtof(specularNode->first_attribute("green")->value(), nullptr),
			strtof(specularNode->first_attribute("blue")->value(), nullptr), 
			strtof(specularNode->first_attribute("shinyness")->value(), nullptr));

		S_MaterialInfo matInfo(materialName.c_str(), ambient, diffuse, specular);

		allMaterials.emplace_back(move(matInfo));
	}

	return allMaterials;
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

	if (!CheckConfigExists(fullPath))
	{
		CreateDefaultConfigsListConfig(fullPath);
	}

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
