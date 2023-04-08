#pragma once

#include "XMLLoader/rapidxml.hpp"
#include "XMLLoader/rapidxml_utils.hpp"
#include <string>
#include "../Globals/stdafx.h"
#include "../Globals/Structs.h"
#include "IConfigInterface.h"

class C_ConfigLoader
{
public:
	C_ConfigLoader(std::string const& inFilename);
	~C_ConfigLoader();

	// Checks if the necessary config directories exist, and makes them if not
	void CheckForDirectories();

	bool CheckConfigExists(std::string const& inConfigFilename) const;

	void CreateDefaultMainConfig();
	void CreateDefaultMaterialsConfig(std::string const& inConfigFilename) const;

	void Initialise();

	int GetSettingValue(SettingType const settingType, char const* settingName) const;
	std::string GetSettingStringValue(SettingType const settingType, char const* settingName) const;
	
	std::vector<MaterialInfo> GetAllMaterials() const;

	ConfigInfo GetConfigByName(char const* const inConfigName) const;

	void ReloadAllConfigs();

private:

	std::string m_ConfigFilename;

	std::vector<SettingNameValuePair> m_AllSettings;

	std::vector<ConfigInfo> m_AllConfigs;
};

