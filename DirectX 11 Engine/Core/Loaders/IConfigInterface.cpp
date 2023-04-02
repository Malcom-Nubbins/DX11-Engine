#include "IConfigInterface.h"

IConfigInterface::IConfigInterface(const std::string& InConfigName)
	: ConfigName(InConfigName)
{
}

IConfigInterface::~IConfigInterface()
{
}

bool IConfigInterface::DoesConfigExist()
{
	struct stat buffer;
	return (stat(ConfigName.c_str(), &buffer) == 0);
}
