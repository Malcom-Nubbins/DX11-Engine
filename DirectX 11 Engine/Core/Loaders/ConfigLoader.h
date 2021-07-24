#pragma once

#include "XMLLoader/rapidxml.hpp"
#include "XMLLoader/rapidxml_utils.hpp"
#include <string>
#include "../Globals/stdafx.h"
#include "../Globals/Structs.h"

class C_ConfigLoader
{
public:
	C_ConfigLoader(std::string const& inFilename);
	~C_ConfigLoader();

	void Initialise();

	int GetSettingValue(SettingType const settingType, char const* settingName) const;
	std::string GetSettingStringValue(SettingType const settingType, char const* settingName) const;
	std::vector<S_TextureInfo> GetAllTextures() const;

	S_ConfigInfo GetConfigByName(char const* const inConfigName) const;

private:

	void InitConfigsList();

	struct S_SettingNameValuePair final
	{
		S_SettingNameValuePair(SettingType const inType, char const* inSettingName, int const inSettingValue, std::string const& inWCharVal)
			: m_Type(inType), m_SettingName(inSettingName), m_SettingValue(inSettingValue), m_SettingValueString(inWCharVal)
		{}

		SettingType m_Type;
		std::string m_SettingName;
		int m_SettingValue;
		std::string m_SettingValueString;
	};

	std::string m_ConfigFilename;

	std::vector<S_SettingNameValuePair> m_AllSettings;

	std::vector<S_ConfigInfo> m_AllConfigs;
};

