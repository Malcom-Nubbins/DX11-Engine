#pragma once

#include "XMLLoader/rapidxml.hpp"
#include "XMLLoader/rapidxml_utils.hpp"
#include <string>
#include "../Globals/stdafx.h"

class C_ConfigLoader
{
public:
	C_ConfigLoader(std::string const& inFilename);
	~C_ConfigLoader();

	void Initialise();

	int GetSettingValue(SettingType const settingType, char const* settingName) const;

private:
	struct S_SettingNameValuePair final
	{
		S_SettingNameValuePair(SettingType const inType, char const* inSettingName, int const inSettingValue)
			: m_Type(inType), m_SettingName(inSettingName), m_SettingValue(inSettingValue)
		{}

		SettingType m_Type;
		std::string m_SettingName;
		int m_SettingValue;
	};

	std::string m_ConfigFilename;

	std::vector<S_SettingNameValuePair> m_AllSettings;
};

