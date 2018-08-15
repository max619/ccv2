/**
* \file      nuiSystemConfiguration.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef NUI_SYSTEM_CONFIGURATION
#define NUI_SYSTEM_CONFIGURATION

#include <json/json.h>

#include <nuiProperty.h>
#include "nuiPluginManager.h"

class nuiSystemConfiguration
{
public:
	static nuiSystemConfiguration* loadConfiguration(std::string path);
	static nuiSystemConfiguration* loadConfiguration(Json::Value& value);
	~nuiSystemConfiguration();

	nuiProperty& property(std::string name);
	int getDynamicLibrariesPathsCount();
	std::string& getDynamicLibraryPath(int index);

private:
	nuiSystemConfiguration();
	std::map<std::string, nuiProperty*> properties;
	std::vector<std::string> libPaths;
};


#endif // !NUI_SYSTEM_CONFIGURATION
