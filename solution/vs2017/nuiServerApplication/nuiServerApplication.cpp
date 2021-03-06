/**
* \file      nuiServerApplication.cpp
* \author    Anatoly Churikov
* \author    Anatoly Lushnikov
* \author    Scott Halstvedt
* \author    Maxim Bagryantsev
* \date      2012-2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/


#include "nuiServerApplication.h"

LOG_DECLARE("CCV");

//! should produce log file
static bool config_syslog = false;

//! update period
int g_config_delay = 200;

//! terminate application
static void signal_term(int signal)
{
	nuiJsonRpcApi::getInstance()->stopApi(true);
}

//! Entrance point
int main(int argc, char **argv)
{
	int exit_ret = 0;
	// Initialize Signals
#ifndef _WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	/// Initialize Debug
	nuiDebugLogger::init(config_syslog);

	nuiFrameworkManager::createInstance();
	nuiFrameworkManager& manager = nuiFrameworkManager::getInstance();

	char* path = argv[0];
	std::string str_path = std::string(path);
	size_t pos = str_path.find_last_of('\\');
	std::string dir = str_path.substr(0, pos) + '\\';
	manager.setStartupPath(dir);

	/// Load defaults
	nuiPluginFrameworkErrorCode::err loadCode = manager.loadDefaultSettings();
	if (loadCode != nuiPluginFrameworkErrorCode::Success)
	{
		LOG(NUI_CRITICAL, "Failed to load defaults");
		return loadCode;
	}

	nuiSystemConfiguration* conf =  manager.getSystemConfiguration();

	/// Initialize API
	if (!nuiJsonRpcApi::getInstance()->init(conf->property("host").asString(), conf->property("port").asInteger()))
	{
		LOG(NUI_CRITICAL, "Failed to Initialize JSON RPC");
		goto exit_critical;
	}
	else
		nuiJsonRpcApi::getInstance()->startApi();


	/// Initialize Framework
	nuiFrameworkManagerErrorCode::err frameworkInitStatus = manager.init();
	if (frameworkInitStatus != nuiFrameworkManagerErrorCode::Success)
	{
		LOG(NUI_CRITICAL, "Failed to initialize framework manager");
	}
	else
		nuiFrameworkManager::getInstance().workflowStart();

	/// Update cycle
	do
	{
		SLEEP(g_config_delay);
	} while (nuiJsonRpcApi::getInstance()->isFinished() == false);


	/// Stop Framework
	manager.workflowStop();

exit_standard:
	/// No cleanup required - Thread is done.
	return exit_ret;

exit_critical:
	exit_ret = 1;
	goto exit_standard;
}