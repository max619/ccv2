
/**
* \file      nuiFrameworkManager.h
* \author    Anatoly Lushnikov
* \author    Scott Halstvedt
* \author    Anatoly Churikov
* \author    Maxim Bagryantsev
* \date      2012-2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/
#pragma once

#ifndef NUI_FRAMEWORK_MANAGER_H
#define NUI_FRAMEWORK_MANAGER_H
#define _WINSOCKAPI_

#include <string>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <fstream>

#include "nuiProperty.h"
#ifndef PT_PASYNC
#define PT_PASYNC

#include "pasync.h"

#endif
#include "nuiTimer.h"
#include "nuiThread.h"
#include "nuiEndpoint.h"
#include "nuiTree.h"
#include "nuiDebugLogger.h"
#include "nuiPlugin.h"
#include "nuiSystemConfiguration.h"

#define USER_CONFIG_PATH "data/user_config.json"
#define SYSTEM_CONFIG_PATH "data/system.json"

class nuiModule;
class nuiModuleDescriptor;
class nuiPipelineModule;
class nuiSystemConfiguration;

//! namespaced enum
struct nuiFrameworkManagerErrorCode
{
	enum err
	{
		Success,
		NonexistentModule,
		InitFailed,
		RootDeletionDeprecated,
	};
};

struct nuiModuleRuntimeDescriptor
{
public:
	double averageFPS;
	double averageWaitTime;
	double averageProcessTime;
	bool isRunning;
};

struct nuiDataStreamRuntimeDescriptor
{
public:
	bool isRunning;
};


//! Provides API for engine manipulations
class nuiFrameworkManager
{
public:
	static nuiFrameworkManager& getInstance();
	static void createInstance();

	//! \todo what does init should mean for framework manager?
	nuiFrameworkManagerErrorCode::err init();

	nuiFrameworkManagerErrorCode::err init(nuiModuleDescriptor* descriptor);

	nuiFrameworkManagerErrorCode::err closePipeline(std::string name);

	//LIST METHODS ===============================================================

	// ++ reviewed
	//! lists modules obtained from Plugins
	std::vector<std::string>* listModules();

	// ++ reviewed
	//! \todo require method to obtain pipelines allowed to be created on current level
	//! lists all the pipelines
	std::vector<std::string>* listPipelines();

	//WORKFLOW CONTROL METHODS ===================================================
	// ++ reviewed
	//! starts the current pipeline (instance chosen by navigate) workflow
	nuiFrameworkManagerErrorCode::err workflowStart();

	// ++ reviewed
	//! starts module index workflow at the current pipeline
	nuiFrameworkManagerErrorCode::err workflowStart(int moduleIndex);

	// ++ reviewed
	//! stops the current pipeline (instance chosen by navigate) workflow
	nuiFrameworkManagerErrorCode::err workflowStop();

	// ++ reviewed
	//! starts module index workflow at the current pipeline
	nuiFrameworkManagerErrorCode::err workflowStop(int moduleIndex);

	//CREATE METHODS =============================================================
	// ++ reviewed
	//! creates pipeline instance at the specified level
	nuiModuleDescriptor* create(const std::string &pipelineName);
	//! ++ reviewed
	//! creates new pipeline template
	nuiModuleDescriptor* createNewPipelineTemplate(const std::string &pipelineName);

	// ++ reviewed
	//! adds module to pipeline template and forces all pipelines already created to get instance of newly created module
	nuiModuleDescriptor* createModule(std::string &pipelineName, std::string &moduleName);

	//! \todo needs better revision
	//! creates connection between modules in specified pipeline and forces all pipelines already created to add instance of that connection
	nuiDataStreamDescriptor* createConnection(std::string &pipelineName,
		int sourceModuleID, int destinationModuleID, int sourcePort, int destinationPort);

	//! \todo needs better revision
	//! sets new input endpoint count on specified pipeline
	int setInputEndpointCount(std::string &pipelineName, int count);

	//! \todo needs better revision
	//! sets new output endpoint count on specified pipeline
	int setOutputEndpointCount(std::string &pipelineName, int count);

	//GET METHODS ================================================================

	// ++ reviewed
	nuiModuleDescriptor *getCurrentPipelineDescriptor();
	// ++ reviewed
	nuiModuleDescriptor *getWorkflowRootDescriptor();
	// ++ reviewed
	nuiModuleDescriptor *getPipelineDescriptor(std::string &pipelineName);
	// ++ reviewed
	nuiModuleDescriptor *getModuleDescriptor(std::string &pipelineName, int index);
	// ++ reviewed
	nuiModuleDescriptor *getModuleDescriptor(std::string &moduleName);

	nuiModule* getModule(std::string &pipelineName, int index);
	// ++ reviewed
	nuiEndpointDescriptor *getInputEndpointDescriptor(std::string &pipelineName, int index);
	// ++ reviewed
	nuiEndpointDescriptor *getOutputEndpointDescriptor(std::string &pipelineName, int index);
	// ++ reviewed
	nuiDataStreamDescriptor *getConnectionDescriptor(std::string &pipelineName,
		int sourceModuleID, int destinationModuleID, int sourcePort, int destinationPort);

	//DELETE METHODS =============================================================

	//! \todo needs better revision; instance level not implemented?
	nuiFrameworkManagerErrorCode::err deletePipeline(std::string &pipelineName);
	//! \todo needs better revision; instance level not implemented?
	nuiModuleDescriptor *deleteModule(std::string &pipelineName, int moduleIndex);
	//! \todo needs better revision; instance level not implemented?
	nuiModuleDescriptor *deleteInputEndpoint(std::string &pipelineName, int index);
	//! \todo needs better revision; instance level not implemented?
	nuiModuleDescriptor *deleteOutputEndpoint(std::string &pipelineName, int index);
	//! \todo needs better revision; instance level not implemented?
	nuiModuleDescriptor *deleteConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID, int sourcePort, int destinationPort);

	//UPDATE METHODS =============================================================

	//! \todo needs better revision
	nuiModuleDescriptor *updatePipeline(std::string &pipelineName, nuiModuleDescriptor* moduleDescriptor);
	//! \todo needs better revision
	nuiModuleDescriptor *updateModule(std::string &pipelineName, int index, nuiModuleDescriptor* moduleDescriptor);
	//! \todo needs better revision
	nuiEndpointDescriptor *updateInputEndpoint(std::string &pipelineName, int index, nuiEndpointDescriptor* endpointDescriptor);
	//! \todo needs better revision
	nuiEndpointDescriptor *updateOutputEndpoint(std::string &pipelineName, int index, nuiEndpointDescriptor* endpointDescriptor);
	//! \todo needs better revision
	nuiDataStreamDescriptor *updateConnection(std::string &pipelineName, int sourceModuleID, int destinationModuleID, int sourcePort, int destinationPort, nuiDataStreamDescriptor *connectionDescriptor);

	//NAVIGATE METHODS ===========================================================

	/** dives into moduleIndex pipeline of the currect pipeline
	*  \returns new current pipeline descriptor, NULL if action failed
	*/
	nuiModuleDescriptor *navigatePush(int moduleIndex);

	/** returns back to upper pipeline
	*  \return new current pipeline descriptor, NULL if action failed
	*/
	nuiModuleDescriptor *navigatePop();

	//OTHER ======================================================================
	//! force nuiPluginManager load default settings and plugins
	nuiPluginFrameworkErrorCode::err loadDefaultSettings();
	nuiPluginFrameworkErrorCode::err loadSettings(std::string path);
	nuiPluginFrameworkErrorCode::err saveSettings(std::string path);

	nuiSystemConfiguration *getSystemConfiguration();

private:
	nuiFrameworkManager();
	nuiFrameworkManager(const nuiFrameworkManager&);
	~nuiFrameworkManager();


	//! gets currently selected pipeline
	nuiPipelineModule *getCurrent();
	//! list of child indexes to the current pipeline
	std::list<int> pathToCurrent;

	nuiModule* currentModule;
	nuiPipelineModule *rootPipeline;
	nuiTree<int, nuiModule*> *dataObjectTree;
	nuiSystemConfiguration *systemConfiguration;

public:
	void setStartupPath(std::string path);
	std::string getRelativeToStartupPath(std::string path);
	std::string getRelativeToStartupPath(char* path);
	static void* getInstancePoiner();

private:
	std::string startupPath;
};



#endif //NUI_FRAMEWORK_MANAGER_H