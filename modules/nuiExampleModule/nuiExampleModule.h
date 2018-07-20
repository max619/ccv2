/** 
 * \file      nuiExampleModule.h
 *            Example Module to be used for developments.
 * \author    Christian Moore
 * \date      2012-2013
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 */

#ifndef NUI_EXAMPLE_MODULE_H
#define NUI_EXAMPLE_MODULE_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(ExampleModule, IplImage*)

class nuiExampleModule : public nuiModule {
public:
	nuiExampleModule(); 
	virtual ~nuiExampleModule();

	void update();
	void start();

private:
    nuiEndpoint* input;
	nuiEndpoint* output;
	nuiDataPacket* outputDataPacket;
    
	IplImage* filterFrame;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiExampleModule)
IMPLEMENT_DEALLOCATOR(nuiExampleModule)

START_IMPLEMENT_DESCRIPTOR(nuiExampleModule,"native","Example Module to be used for developments.")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor,0);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor,0);
descriptor->property("use_thread").set(false);
END_IMPLEMENT_DESCRIPTOR(nuiExampleModule)	

START_EXPORT_MODULES()	  
REGISTER_MODULE(nuiExampleModule,"nuiExampleModule",1,0)
END_EXPORT_MODULES()

#endif

