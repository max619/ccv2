/**
 * \file      nuiPluginManager.cpp
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \author    Maxim Bagryantsev
 * \date      2012-2018
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 * \copyright Copyright 2018 Appollo Pro. All rights reserved.
 */

#include "nuiDynamicLibrary.h"
#include "nuiPluginManager.h"
#include "nuiModule.h"
#include "nuiProperty.h"

#include <string>
#include <fstream>

nuiPluginManager& nuiPluginManager::getInstance()
{
	static nuiPluginManager instance;
	return instance;
};

nuiPluginManager::nuiPluginManager()
{
	pluginFrameworkService.version.major = MAJOR_VERSION;
	pluginFrameworkService.version.minor = MINOR_VERSION;
	pluginFrameworkService.registerModule = registerModule;
	pluginFrameworkService.frameworkPointer = nuiFrameworkManager::getInstancePoiner();

	loadingPlugin = NULL;
};

nuiPluginManager::nuiPluginManager(const nuiPluginManager&)
{

};

//! \todo destructor realization needed?
// nuiPluginManager::~nuiPluginManager()
// {
//   shutdown();
// };

nuiPluginFrameworkErrorCode::err nuiPluginManager::shutdown()
{
	// 	nuiPluginFrameworkErrorCode result = nuiPluginFrameworkOK;
	//     nuiDynamicLibraryMap::iterator i;
	// 	for ( i = dynamicLibraryMap.begin(); i != dynamicLibraryMap.end(); i++)
	// 		unloadLibrary(i->second);
	//     nuiDynamicLibraryFreeVec::iterator func;
	// 
	// 	for (func = dynamicLibraryFreeVector.begin(); func != dynamicLibraryFreeVector.end(); ++func)
	// 	{
	// 		try	{  result = (*func)();	}
	// 		catch (...)	{  result = nuiDynamicLibraryAlreadyUnloaded;	}
	// 	}
	// 
	// 	registerPluginParamsMap.clear();
	// 	dynamicLibraryFreeVector.clear();
	// 	return result;
	return nuiPluginFrameworkErrorCode::Success;
};

bool nuiPluginManager::validate(const nuiRegisterModuleParameters *params)
{
	return
		params &&
		params->allocateFunc &&
		params->deallocateFunc &&
		params->getDescriptorFunc;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::registerModule(const nuiRegisterModuleParameters *params)
{
	nuiPluginManager& pm = nuiPluginManager::getInstance();

	if (!pm.loadingPlugin)
		return nuiPluginFrameworkErrorCode::UnexpectedError;
	//if unexpected error occured

	if (!nuiPluginManager::validate(params))
		return nuiPluginFrameworkErrorCode::ModuleRegistrationFailed;

	nuiPluginFrameworkVersion version = pm.pluginFrameworkService.version;

	if (version.major != params->version.major)
		return nuiPluginFrameworkErrorCode::IncompatibleVersion;
	// perform checks whether we can register module

	for (int i = 0; i < pm.modulesLoaded.size(); i++)
	{
		if (params->name == pm.modulesLoaded[i]->name)
		{
			return nuiPluginFrameworkErrorCode::RepeatingModule;
		}
	}
	// check if already registered module

	nuiModuleLoaded* module = new nuiModuleLoaded(params);
	module->setParentPlugin(pm.loadingPlugin);
	// create wrapper for module management

	return nuiPluginFrameworkErrorCode::Success;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::unloadModule(std::string name)
{
	for (int i = modulesLoaded.size() - 1; i >= 0; i--)
		if (modulesLoaded[i]->name == name)
		{
			modulesLoaded[i]->clearInstances();
			modulesLoaded[i]->unregisterParent();
			modulesLoaded.erase(modulesLoaded.begin() + i);
		}
	return nuiPluginFrameworkErrorCode::Success;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadLibrary(const std::string path)
{
	if (loadingPlugin)
		return nuiPluginFrameworkErrorCode::UnexpectedError;

	//! \todo better error handling - consider using something better than string
	std::string errorString;
	nuiDynamicLibrary *library = nuiDynamicLibrary::load(path, errorString);
	if (!library)
		return nuiPluginFrameworkErrorCode::PluginLoadingFailed;
	else
	{
		loadingPlugin = new nuiPluginLoaded(library);
	}

	nuiLibraryLoadFunc initFunc =
		(nuiLibraryLoadFunc)(library->getSymbol("nuiLibraryLoad"));

	if (initFunc == NULL)
	{
		loadingPlugin = NULL;
		return nuiPluginFrameworkErrorCode::EntryPointNotFound;
	}

	//send plugin framework service to plugin so modules could register themselves
	nuiPluginFrameworkErrorCode::err error = initFunc(&pluginFrameworkService);

	if (error == nuiPluginFrameworkErrorCode::Success)
	{
		// move loaded modules to modules
		modulesLoaded.insert(modulesLoaded.end(),
			loadingPlugin->loadedModules.begin(),
			loadingPlugin->loadedModules.end());
		pluginsLoaded.push_back(loadingPlugin);
	}
	// check whether plugins were loaded correctly

	loadingPlugin = NULL;

	return nuiPluginFrameworkErrorCode::Success;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::unloadLibrary(const std::string path)
{
	for (int i = pluginsLoaded.size() - 1; i >= 0; i--)
	{
		std::vector<nuiModuleLoaded*>& modules = pluginsLoaded[i]->loadedModules;
		for (int j = modules.size() - 1; j >= 0; j--)
		{
			unloadModule(modules[j]->name);
		}
	}
	// unload all loaded modules

	for (int i = pluginsLoaded.size() - 1; i >= 0; i--)
	{
		std::vector<nuiModuleLoaded*>& modules = pluginsLoaded[i]->loadedModules;
		for (int j = modules.size() - 1; j >= 0; j--)
		{
			unloadModule(modules[j]->name);
		}
	}

	return nuiPluginFrameworkErrorCode::Success;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadDefaultConfiguration()
{
	nuiFrameworkManager& frameworkManager = nuiFrameworkManager::getInstance();
	if (nuiUtils::isFileExists(frameworkManager.getRelativeToStartupPath(USER_CONFIG_PATH)))
		return loadConfiguration(USER_CONFIG_PATH);
	else
		return loadConfiguration("data/default_config.json");
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadConfiguration(std::string path)
{
	nuiFrameworkManager& frameworkManager = nuiFrameworkManager::getInstance();

	std::ifstream settingsFile(frameworkManager.getRelativeToStartupPath(path));
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(settingsFile, root);
	if (!parsingSuccessful)
		return nuiPluginFrameworkErrorCode::DefaultSettingsCorrupted;	

	nuiSystemConfiguration* sysCfg = frameworkManager.getSystemConfiguration();
	for (int i = 0; i < sysCfg->getDynamicLibrariesPathsCount(); i++)
	{
		std::string path = frameworkManager.getRelativeToStartupPath(sysCfg->getDynamicLibraryPath(i));
		this->loadLibrary(path);
	}

	Json::Value pipelines = root.get("pipelines", NULL);
	this->loadPipelines(&pipelines);

	return nuiPluginFrameworkErrorCode::Success;
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadPipelines(Json::Value* pipelines)
{
	for (Json::Value::iterator i = pipelines->begin(); i != pipelines->end(); i++)
	{
		//! \todo maybe should continue loading if has error?
		nuiModuleDescriptor* pipelineDescr = loadPipeline(&*i);
		if (!pipelineDescr)
			return nuiPluginFrameworkErrorCode::UnexpectedError;
		// check for NULL-ness

		for (int i = 0; i < modulesLoaded.size(); i++)
		{
			if (pipelineDescr->getName() == modulesLoaded[i]->name)
				return nuiPluginFrameworkErrorCode::RepeatingModule;
		}
		for (int i = 0; i < pipelinesLoaded.size(); i++)
		{
			if (pipelineDescr->getName() == pipelinesLoaded[i]->getName())
				return nuiPluginFrameworkErrorCode::RepeatingModule;
		}
		// check for duplicates

		pipelinesLoaded.push_back(pipelineDescr);
	}

	return nuiPluginFrameworkErrorCode::Success;
};

nuiModuleDescriptor* nuiPluginManager::loadPipeline(Json::Value* root)
{
	nuiModuleDescriptor* moduleDescriptor = new nuiModuleDescriptor();
	moduleDescriptor->setName(root->get("type", NULL).asString());
	moduleDescriptor->setAuthor(root->get("author", NULL).asString());
	moduleDescriptor->setDescription(root->get("description", NULL).asString());

	//! \todo what for? why 0x0FFFFFFF not 0xFFFFFFFF?! wtf?!
	const int PIPELINE_ID = 0x0FFFFFFF;

	moduleDescriptor->property("id") = *(new nuiProperty(PIPELINE_ID));
	// setting this strange id

	parseDescriptorProperties(moduleDescriptor, root);
	// parsing other properties

	Json::Value submodules = root->get("modules", NULL);
	if (submodules.isArray()) {
		for (Json::Value::iterator i = submodules.begin(); i != submodules.end(); i++)
		{
			nuiModuleDescriptor *childDescriptor = new nuiModuleDescriptor();
			childDescriptor->setName((*i).get("type", NULL).asString());
			int id = (*i).get("id", PIPELINE_ID).asInt();
			childDescriptor->property("id") = *(new nuiProperty(id));
			parseDescriptorProperties(childDescriptor, &*i);
			moduleDescriptor->addChildModuleDescriptor(childDescriptor);
		}
	}
	// extracting child module descriptor settings

	Json::Value endpoints = root->get("endpoints", new Json::Value);

	Json::Value inputs = endpoints.get("input", new Json::Value);
	for (Json::Value::iterator i = inputs.begin(); i != inputs.end(); i++)
	{
		nuiEndpointDescriptor* endpointDescriptor = new nuiEndpointDescriptor((*i).get("type", "*").asString());
		endpointDescriptor->setIndex((*i).get("id", 0).asInt()); // is this the right way to index?
		moduleDescriptor->addInputEndpointDescriptor(endpointDescriptor, endpointDescriptor->getIndex());
	}
	// get input endpoints
	Json::Value outputs = endpoints.get("output", new Json::Value);
	for (Json::Value::iterator i = outputs.begin(); i != outputs.end(); i++)
	{
		nuiEndpointDescriptor* endpointDescriptor = new nuiEndpointDescriptor((*i).get("type", "*").asString());
		endpointDescriptor->setIndex((*i).get("id", 0).asInt()); // is this the right way to index?
		moduleDescriptor->addOutputEndpointDescriptor(endpointDescriptor, endpointDescriptor->getIndex());
	}
	// get output endpoints

	Json::Value connections = root->get("connections", new Json::Value);
	for (Json::Value::iterator i = connections.begin(); i != connections.end(); i++)
	{
		nuiDataStreamDescriptor *datastreamDescriptor = new nuiDataStreamDescriptor();
		int sourceID = (*i).get("source", new Json::Value).get("id", -1).asInt();
		int destID = (*i).get("destination", new Json::Value).get("id", -1).asInt();
		int sourcePort = (*i).get("source", new Json::Value).get("port", -1).asInt();
		int destPort = (*i).get("destination", new Json::Value).get("port", -1).asInt();

		if ((sourceID != -1) && (destID != -1) && (sourcePort != -1) && (destPort != -1))
		{
			datastreamDescriptor->sourceModuleID = sourceID;
			datastreamDescriptor->sourcePort = sourcePort;
			datastreamDescriptor->destinationModuleID = destID;
			datastreamDescriptor->destinationPort = destPort;
			Json::Value props = (*i).get("properties", NULL);
			if (props != NULL) {
				datastreamDescriptor->asyncMode = props.get("async", 0).asBool();
				datastreamDescriptor->buffered = props.get("buffered", 0).asBool();
				datastreamDescriptor->bufferSize = props.get("buffersize", 0).asInt();
				datastreamDescriptor->deepCopy = props.get("deepcopy", 0).asBool();
				datastreamDescriptor->lastPacket = props.get("lastpacket", 0).asBool();
				datastreamDescriptor->overflow = props.get("overflow", 0).asBool();
			}
			moduleDescriptor->addDataStreamDescriptor(datastreamDescriptor);
		}
	}
	// get connections

	return moduleDescriptor;
};

Json::Value nuiPluginManager::serializePipeline(nuiModuleDescriptor* desc)
{
	Json::Value pipeline;
	pipeline["type"] = desc->getName();
	pipeline["author"] = desc->getAuthor();
	pipeline["description"] = desc->getDescription();
	writeDescriptorProperties(desc, pipeline);

	Json::Value submodules;

	for (int i = 0; i < desc->getChildModulesCount(); i++)
	{
		nuiModuleDescriptor* childModule = desc->getChildModuleDescriptor(i);
		Json::Value submodule;
		submodule["type"] = childModule->getName();
		submodule["id"] = childModule->property("id").asInteger();
		writeDescriptorProperties(childModule, submodule);
		submodules.append(submodule);
	}
	pipeline["modules"] = submodules;

	// extracting child module descriptor settings

	Json::Value endpoints;

	Json::Value inputs;

	for (int i = 0; i < desc->getInputEndpointsCount(); i++)
	{
		nuiEndpointDescriptor* endpoint = desc->getInputEndpointDescriptor(i);
		Json::Value input;
		input["type"] = endpoint->getDescriptor();
		input["id"] = endpoint->getIndex();
		inputs.append(input);
	}
	endpoints["input"] = inputs;
	// get input endpoints
	Json::Value outputs;
	for (int i = 0; i < desc->getOutputEndpointsCount(); i++)
	{
		nuiEndpointDescriptor* endpoint = desc->getOutputEndpointDescriptor(i);
		Json::Value output;
		output["type"] = endpoint->getDescriptor();
		output["id"] = endpoint->getIndex();
		outputs.append(output);
	}
	endpoints["output"] = outputs;
	pipeline["endpoints"] = endpoints;
	// get output endpoints

	Json::Value connections;
	for (int i = 0; i < desc->getDataStreamDescriptorCount(); i++)
	{
		nuiDataStreamDescriptor *datastreamDescriptor = desc->getDataStreamDescriptor(i);
		Json::Value connection;
		Json::Value source;
		Json::Value destination;
		Json::Value properties;

		source["id"] = datastreamDescriptor->sourceModuleID;
		source["port"] = datastreamDescriptor->sourcePort;

		destination["id"] = datastreamDescriptor->destinationModuleID;
		destination["port"] = datastreamDescriptor->destinationPort;

		properties["async"] = datastreamDescriptor->asyncMode;
		properties["buffered"] = datastreamDescriptor->buffered;
		properties["buffersize"] = datastreamDescriptor->bufferSize;
		properties["deepcopy"] = datastreamDescriptor->deepCopy;
		properties["lastpacket"] = datastreamDescriptor->lastPacket;
		properties["overflow"] = datastreamDescriptor->overflow;


		connection["source"] = source;
		connection["destination"] = destination;
		connection["properties"] = properties;
		connections.append(connection);
	}
	pipeline["connections"] = connections;
	// get connections

	return pipeline;
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::unloadPipeline(const std::string& name)
{
	return nuiPluginFrameworkErrorCode::Success;
};

void nuiPluginManager::parseDescriptorProperties(nuiModuleDescriptor* moduleDescriptor, Json::Value* root)
{
	std::map<std::string, nuiProperty*>& props = moduleDescriptor->getProperties();
	Json::Value pValue = root->get("properties", Json::Value());
	nuiPluginManager::deserializePropertiesToMap(props, pValue);
};

void nuiPluginManager::writeDescriptorProperties(nuiModuleDescriptor* moduleDescriptor, Json::Value& root)
{
	Json::Value properties;
	for (std::map<std::string, nuiProperty*>::iterator it = moduleDescriptor->getProperties().begin(); it != moduleDescriptor->getProperties().end(); it++)
	{
		//properties[it->first] = asString();
		nuiProperty* prop = it->second;
		switch (prop->getType())
		{
		case NUI_PROPERTY_BOOL:
			properties[it->first] = prop->asBool();
			break;
		case NUI_PROPERTY_INTEGER:
			properties[it->first] = prop->asInteger();
			break;
		case NUI_PROPERTY_DOUBLE:
			properties[it->first] = prop->asDouble();
			break;
		case NUI_PROPERTY_FLOAT:
			properties[it->first] = prop->asFloat();
			break;
		default:
			properties[it->first] = prop->asString();
			break;

		}
	}
	root["properties"] = properties;
}


std::vector<std::string>* nuiPluginManager::listLoadedModules()
{
	std::vector<std::string>* modules = new std::vector<std::string>();
	for (int i = 0; i < this->modulesLoaded.size(); i++)
		modules->push_back(this->modulesLoaded[i]->name);
	return modules;
}

std::vector<std::string>* nuiPluginManager::listLoadedPipelines()
{
	std::vector<std::string>* pipelines = new std::vector<std::string>();
	for (int i = 0; i < this->pipelinesLoaded.size(); i++)
		pipelines->push_back(this->pipelinesLoaded[i]->getName());
	return pipelines;
}

nuiModuleDescriptor* nuiPluginManager::getDescriptor(const std::string name)
{
	std::vector<nuiModuleLoaded*>::iterator it1;
	for (it1 = modulesLoaded.begin(); it1 != modulesLoaded.end(); it1++)
	{
		if ((*it1)->name == name)
			return (*it1)->getDescriptor();
	}
	std::vector<nuiModuleDescriptor*>::iterator it2;
	for (it2 = pipelinesLoaded.begin(); it2 != pipelinesLoaded.end(); it2++)
	{
		if ((*it2)->getName() == name)
			return (*it2);
	}
	//trying to find such name among modules or pipelines

	return NULL;
	//if search resulted in nothing;
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::registerPipeline(nuiModuleDescriptor* descr)
{

	std::vector<nuiModuleDescriptor*>::iterator it;
	for (it = pipelinesLoaded.begin(); it != pipelinesLoaded.end(); it++)
	{
		if ((*it)->getName() == descr->getName())
			return nuiPluginFrameworkErrorCode::RepeatingModule;
	}
	pipelinesLoaded.push_back(descr);
	return nuiPluginFrameworkErrorCode::Success;
}

nuiPluginFrameworkErrorCode::err nuiPluginManager::unregisterPipeline(std::string name)
{
	std::vector<nuiModuleDescriptor*>::iterator it;
	for (it = pipelinesLoaded.begin(); it != pipelinesLoaded.end(); it++)
	{
		if ((*it)->getName() == name)
		{
			nuiModuleDescriptor* old = (*it);
			pipelinesLoaded.erase(it);
			delete old;
			return nuiPluginFrameworkErrorCode::Success;
		}
	}
	return nuiPluginFrameworkErrorCode::UnexpectedError;
}

nuiModuleLoaded* nuiPluginManager::getLoadedModule(const std::string name)
{
	std::vector<nuiModuleLoaded*>::iterator it;
	for (it = modulesLoaded.begin(); it != modulesLoaded.end(); it++)
	{
		if (name == (*it)->name)
			return *it;
	}
	return NULL;
}

std::vector<nuiModuleLoaded*>& nuiPluginManager::getLoadedModules()
{
	return modulesLoaded;
}

//int nuiPluginManager::getDynamicLibrariesPathsCount()
//{
//	return dynamicLibrariesPaths.size();;
//}

//std::string& nuiPluginManager::getDynamicLibraryPath(int index)
//{
//	return dynamicLibrariesPaths.at(index);
//}

std::vector<nuiModuleDescriptor*>* nuiPluginManager::getPipelineDescriptors()
{
	return &pipelinesLoaded;
}

Json::Value nuiPluginManager::getCurrentConfiguration()
{
	Json::Value cfgRoot;
	std::vector<std::string>* pipelines = nuiFrameworkManager::getInstance().listPipelines();

	/*Json::Value modules;

	for (int i = 0; i < nuiPluginManager::getInstance().getDynamicLibrariesPathsCount(); i++)
	{
		Json::Value module;
		std::string& path = nuiPluginManager::getInstance().getDynamicLibraryPath(i);
		module["path"] = path;
		modules.append(module);
	}
	cfgRoot["module_library"] = modules;*/

	nuiPluginManager::getInstance().listLoadedModules();

	Json::Value pipes;

	for (std::vector<std::string>::iterator it = pipelines->begin(); it < pipelines->end(); it++)
	{
		std::string pName = *it;

		nuiModuleDescriptor* pipeline = nuiFrameworkManager::getInstance().getPipelineDescriptor(pName);
		pipes.append(nuiPluginManager::getInstance().serializePipeline(pipeline));
	}
	cfgRoot["pipelines"] = pipes;

	return cfgRoot;
}

void nuiPluginManager::writeJsonToFile(Json::Value & value, std::string path)
{
	std::ofstream settingsFile(nuiFrameworkManager::getInstance().getRelativeToStartupPath(path));
	Json::StyledStreamWriter writer;
	writer.write(settingsFile, value);
}

void nuiPluginManager::deserializePropertiesToMap(std::map<std::string, nuiProperty*>& props, Json::Value root)
{

	Json::Value::Members propertyNames = root.getMemberNames();
	// get list of properties' names

	Json::Value defaultval;

	for (Json::Value::Members::iterator i = propertyNames.begin(); i != propertyNames.end(); i++)
	{
		std::string propertyID = *i;
		// get single property name
		Json::Value value = root.get(*i, defaultval);
		// get property value


		if ((value != defaultval) && (propertyID != "none")) // if both are present
		{
			switch (value.type())
			{
			case Json::ValueType::intValue:
			case Json::ValueType::uintValue:
				props[propertyID] = new nuiProperty(NUI_PROPERTY_INTEGER);
				props[propertyID]->set(value.asInt());
				break;     ///< unsigned integer value
			case Json::ValueType::realValue:
				props[propertyID] = new nuiProperty(NUI_PROPERTY_DOUBLE);
				props[propertyID]->set(value.asDouble());
				break;     ///< double value
			case Json::ValueType::stringValue:
				props[propertyID] = new nuiProperty(NUI_PROPERTY_STRING);
				props[propertyID]->set(value.asString());
				break;   ///< UTF-8 string value
			case Json::ValueType::booleanValue:
				props[propertyID] = new nuiProperty(NUI_PROPERTY_BOOL);
				props[propertyID]->set(value.asBool());
				break;  ///< bool value				
			default:
				break;
			}
			// save the property to descriptor's property list
		}
	}
}
