#include "nuiDynamicLibrary.h"
#include "nuiPluginManager.h"
#include "nuiModule.h"
#include "nuiProperty.h"

#include <string>
#include <fstream>

#ifdef WIN32
#include <guiddef.h>
#include <cguid.h>
#else
  //! \todo other include headers
#endif

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
  
  if(!pm.loadingPlugin)
    return nuiPluginFrameworkErrorCode::UnexpectedError;
  //if unexpected error occured
  
  if (!nuiPluginManager::validate(params))
    return nuiPluginFrameworkErrorCode::ModuleRegistrationFailed;

  nuiPluginFrameworkVersion version = pm.pluginFrameworkService.version;

  if (version.major != params->version.major)
    return nuiPluginFrameworkErrorCode::IncompatibleVersion;
  // perform checks whether we can register module

  for (int i=0; i<pm.modulesLoaded.size(); i++)
  {
    if(params->name == pm.modulesLoaded[i]->name )
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
  for(int i=modulesLoaded.size()-1 ; i>=0 ; i--)
    if(modulesLoaded[i]->name == name)
    {
      modulesLoaded[i]->clearInstances();
      modulesLoaded[i]->unregisterParent();
      modulesLoaded.erase(modulesLoaded.begin() + i);
    }
  return nuiPluginFrameworkErrorCode::Success;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadLibrary(const std::string path)
{
  if(loadingPlugin)
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
  
  if(error == nuiPluginFrameworkErrorCode::Success)
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
  for(int i=pluginsLoaded.size()-1; i>=0 ; i--)
  {
    std::vector<nuiModuleLoaded*>& modules = pluginsLoaded[i]->loadedModules;
    for(int j=modules.size()-1 ; j>=0 ; j--)
    {
      unloadModule(modules[j]->name);
    }
  }
  // unload all loaded modules
  
  for(int i=pluginsLoaded.size()-1; i>=0 ; i--)
  {
    std::vector<nuiModuleLoaded*>& modules = pluginsLoaded[i]->loadedModules;
    for(int j=modules.size()-1 ; j>=0 ; j--)
    {
      unloadModule(modules[j]->name);
    }
  }


  return nuiPluginFrameworkErrorCode::Success;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadDefaultConfiguration()
{
  char* fileName = "data/default_config.json";
  std::ifstream settingsFile(fileName);
  Json::Value root;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(settingsFile, root);
  if(!parsingSuccessful) 
    return nuiPluginFrameworkErrorCode::DefaultSettingsCorrupted;

  Json::Value modules = root.get("module_library", NULL);
  for (Json::Value::iterator i = modules.begin(); i != modules.end(); i++) 
  {
    std::string path = (*i).get("path", NULL).asString();
    this->loadLibrary(path);
  }
  
  this->loadPipelines(root.get("pipelines", NULL));

  return nuiPluginFrameworkErrorCode::Success;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::loadPipelines( Json::Value& pipelines )
{
  for(Json::Value::iterator i = pipelines.begin(); i != pipelines.end(); i++) 
  {
    this->loadPipeline(*i);
    // check if already loaded by GUID
    // save or refuse
  }

  return nuiPluginFrameworkErrorCode::Success;
};

nuiModuleDescriptor* nuiPluginManager::loadPipeline(Json::Value& root)
{
  nuiModuleDescriptor* moduleDescriptor = new nuiModuleDescriptor();
  moduleDescriptor->setName(root.get("type", NULL).asString());
  moduleDescriptor->setAuthor(root.get("author", NULL).asString());
  moduleDescriptor->setDescription(root.get("description", NULL).asString());
  
  //! \todo what for? why 0x0FFFFFFF not 0xFFFFFFFF?! wtf?!
  const int PIPELINE_ID = 0x0FFFFFFF;
  
  moduleDescriptor->property("id") = *(new nuiProperty(PIPELINE_ID));
  // setting this strange id

  parseDescriptor(*moduleDescriptor, root);
  // parsing other properties
  
  Json::Value submodules = root.get("modules", NULL);
  if(submodules.isArray()) {
    for (Json::Value::iterator i = submodules.begin(); i!=submodules.end(); i++)
    {
      nuiModuleDescriptor *childDescriptor = new nuiModuleDescriptor();
      childDescriptor->setName((*i).get("type", NULL).asString());
      int id = (*i).get("id", PIPELINE_ID).asInt();
      childDescriptor->property("id") = *(new nuiProperty(id));
      parseDescriptor(*childDescriptor, *i);
      moduleDescriptor->addChildModuleDescriptor(childDescriptor);
    }
  }
  // extracting child module descriptor settings
  
  Json::Value endpoints = root.get("endpoints", new Json::Value);

  Json::Value inputs = endpoints.get("input", new Json::Value);
  for (Json::Value::iterator i = inputs.begin(); i!=inputs.end(); i++)
  {
    nuiEndpointDescriptor* endpointDescriptor = new nuiEndpointDescriptor((*i).get("type", "*").asString());
    endpointDescriptor->setIndex((*i).get("id", 0).asInt()); // is this the right way to index?
    moduleDescriptor->addInputEndpointDescriptor(endpointDescriptor, endpointDescriptor->getIndex());
  }
  // get input endpoints
  Json::Value outputs = endpoints.get("output", new Json::Value);
  for (Json::Value::iterator i = outputs.begin(); i!=outputs.end(); i++)
  {
    nuiEndpointDescriptor* endpointDescriptor = new nuiEndpointDescriptor((*i).get("type", "*").asString());
    endpointDescriptor->setIndex((*i).get("id", 0).asInt()); // is this the right way to index?
    moduleDescriptor->addOutputEndpointDescriptor(endpointDescriptor, endpointDescriptor->getIndex());
  }
  // get output endpoints

  Json::Value connections = root.get("connections", new Json::Value);
  for (Json::Value::iterator i = connections.begin(); i!=connections.end(); i++)
  {
    nuiDataStreamDescriptor *datastreamDescriptor = new nuiDataStreamDescriptor();
    int sourceID = (*i).get("source", new Json::Value).get("id", -1).asInt();
    int destID = (*i).get("destination", new Json::Value).get("id", -1).asInt();
    int sourcePort = (*i).get("source", new Json::Value).get("port", -1).asInt();
    int destPort = (*i).get("destination", new Json::Value).get("port", -1).asInt();

    if ((sourceID!=-1) && (destID!=-1) && (sourcePort!=-1) &&(destPort!=-1))
    {
      datastreamDescriptor->sourceModuleID = sourceID;
      datastreamDescriptor->sourcePort = sourcePort;
      datastreamDescriptor->destinationModuleID = destID;
      datastreamDescriptor->destinationPort = destPort;
      Json::Value props = (*i).get("properties", NULL);
      if(props != NULL) {
        datastreamDescriptor->asyncMode  = props.get("async",0).asBool();
        datastreamDescriptor->buffered  = props.get("buffered",0).asBool();
        datastreamDescriptor->bufferSize  = props.get("buffersize",0).asInt();
        datastreamDescriptor->deepCopy  = props.get("deepcopy",0).asBool();
        datastreamDescriptor->lastPacket = props.get("lastpacket",0).asBool();
        datastreamDescriptor->overflow = props.get("overflow",0).asBool();
      }
      moduleDescriptor->addDataStreamDescriptor(datastreamDescriptor);
    }
  }
  // get connections

  return moduleDescriptor;
};

nuiPluginFrameworkErrorCode::err nuiPluginManager::unloadPipeline( const GUID& guid )
{
  return nuiPluginFrameworkErrorCode::Success;
};

void nuiPluginManager::parseDescriptor( nuiModuleDescriptor &moduleDescriptor, const Json::Value& root )
{
  Json::Value properties = root.get("properties", new Json::Value);
  // extracting props from "properties" json value
  
  Json::Value::Members propertyNames = properties.getMemberNames();
  // get list of properties' names

  for (Json::Value::Members::iterator i = propertyNames.begin(); i!=propertyNames.end(); i++)
  {
    std::string propertyID = *i;
    // get single property name
    Json::Value value = properties.get(*i, "none");
    // get property value

    if ((value != "none") && (propertyID != "none")) // if both are present
    {
      if (moduleDescriptor.getProperties().find(propertyID) == 
        moduleDescriptor.getProperties().end()) // if not found property
      {
        if(value.isInt())
          moduleDescriptor.property(propertyID).set(value.asInt());
        else 
        {
          if(value.isString()) 
            moduleDescriptor.property(propertyID).set(value.asString());
        }
        // save the property to descriptor's property list
      }
    }
  }
};
