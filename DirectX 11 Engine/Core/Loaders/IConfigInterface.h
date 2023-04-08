#pragma once
#include "../Globals/stdafx.h"

#include "../Core/Loaders/XMLLoader/rapidxml.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_ext.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_print.hpp"
#include "../Core/Loaders/XMLLoader/rapidxml_utils.hpp"

struct SettingNameValuePair final
{
	SettingNameValuePair(SettingType const inType, char const* inSettingName, int const inSettingValue, std::string const& inWCharVal)
		: m_Type(inType), m_SettingName(inSettingName), m_SettingValue(inSettingValue), m_SettingValueString(inWCharVal)
	{}

	SettingType m_Type;
	std::string m_SettingName;
	int m_SettingValue;
	std::string m_SettingValueString;
};

class IConfigInterface
{
public:
	IConfigInterface(const std::string& InConfigName);
	virtual ~IConfigInterface() = 0;

	const std::string& GetConfigName() const { return ConfigName; }

protected:

	virtual void CreateConfig() = 0;
	virtual void LoadConfig() = 0;
	bool DoesConfigExist();

private:
	std::string ConfigName;
};

