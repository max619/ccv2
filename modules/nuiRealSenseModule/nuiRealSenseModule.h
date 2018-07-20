#pragma once

/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiExampleModule.h
// Purpose:     Example Module to be used for developments.
// Author:      Christian Moore
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_REALSENSE_MODULE_H
#define NUI_REALSENSE_MODULE_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(RealSenseModule, IplImage*)

class nuiRealSenseModule : public nuiModule {
public:
	nuiRealSenseModule();
	virtual ~nuiRealSenseModule();

	void update();
	void start();

private:
	nuiEndpoint* output;
	nuiDataPacket* outputDataPacket;

	cv::Mat frame;
	IplImage* img;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiRealSenseModule)
IMPLEMENT_DEALLOCATOR(nuiRealSenseModule)

START_IMPLEMENT_DESCRIPTOR(nuiRealSenseModule, "native", "Get video data from realsense camera")
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor, 0);
//descriptor->property("use_thread").set(true);
//descriptor->property("oscillator_mode").set(true);
//descriptor->property("oscillator_wait").set(50);
END_IMPLEMENT_DESCRIPTOR(nuiRealSenseModule)

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()
REGISTER_PLUGIN(nuiRealSenseModule, "nuiRealSenseModule", 1, 0)
END_MODULE_REGISTRATION()

#endif

