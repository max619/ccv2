/**
* \file      nuiSystemConfiguration.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiSystemConfiguration.h"


nuiSystemConfiguration::nuiSystemConfiguration()
{
}

nuiSystemConfiguration * nuiSystemConfiguration::loadConfiguration(std::string path)
{
	std::ifstream f(path);
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(f, root))
	{
		return nuiSystemConfiguration::loadConfiguration(root);
	}
	else
	{
		return NULL;
	}
}

nuiSystemConfiguration * nuiSystemConfiguration::loadConfiguration(Json::Value & value)
{
	nuiSystemConfiguration* conf = new nuiSystemConfiguration();
	nuiPluginManager::deserializePropertiesToMap(conf->properties, value["properties"]);
	Json::Value& modules = value.get("module_library", Json::Value());
	if (modules.isArray())
	{
		for (Json::Value::iterator it = modules.begin(); it != modules.end(); it++)
		{
			Json::Value path = (*it).get("path", NULL);
			if (path != NULL)
				conf->libPaths.push_back(path.asString());
		}
	}
	return conf;
}

nuiSystemConfiguration::~nuiSystemConfiguration()
{
}

nuiProperty & nuiSystemConfiguration::property(std::string str)
{
	std::map<std::string, nuiProperty*>::iterator it;
	it = this->properties.find(str);
	if (it == this->properties.end()) {
		this->properties[str] = new nuiProperty("", "?? auto created ??");
		return *(this->properties[str]);
	}
	return *it->second;
}

int nuiSystemConfiguration::getDynamicLibrariesPathsCount()
{
	return libPaths.size();
}

std::string & nuiSystemConfiguration::getDynamicLibraryPath(int index)
{
	return libPaths.at(index);
}
