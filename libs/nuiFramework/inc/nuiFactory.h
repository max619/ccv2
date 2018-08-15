/**
 * \file      nuiFactory.h
 * \author    Scott Halstvedt
 * \author    Anatoly Lushnikov
 * \author    Anatoly Churikov
 * \author    Maxim Bagryantsev
 * \date      2012-2018
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 * \copyright Copyright 2018 Appollo Pro. All rights reserved.
 */

#ifndef NUI_FACTORY_H
#define NUI_FACTORY_H

#include <vector>
#include <map>
#include <string>
#include <list>

#include "nuiDebugLogger.h"
#include "nuiModule.h"
#include "nuiPluginManager.h"

 /** \class nuiFactory
  *  \brief Provides methods for plugin creation and removal
  *  Singleton class. Provides methods for plugin creation and removal
  *  Also allows to get descriptors for already created plugins
  */
class nuiFactory
{
public:
	//! Singleton wrapper
	static nuiFactory& getInstance();

	//! Set module instance properties just like in descriptor
	static void applyDescriptorProps(nuiModule* module, nuiModuleDescriptor* descriptor);

	//! lists available pipeline names
	std::vector<std::string>* listPipelines();
	//! lists available module names
	std::vector<std::string>* listModules();

	//! Creates pipeline or module with specified name
	nuiModule* create(const std::string& moduleName);

	//! registers pipeline descriptor
	nuiPluginFrameworkErrorCode::err registerPipelineDescriptor(nuiModuleDescriptor* descriptor);

	//! Creates pipeline given pipeline descriptor
	nuiModule* createPipeline(nuiModuleDescriptor* descriptor);
	//! Creates module given module descriptor
	nuiModule* createModule(nuiModuleLoaded* module);

private:
	nuiFactory();
	nuiFactory(const nuiFactory&);

};

#endif //NUI_FACTORY_H