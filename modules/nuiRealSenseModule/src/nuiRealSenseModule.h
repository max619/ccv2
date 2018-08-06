/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiRealSenseModule.h
// Purpose:     Intel Realsense cameras IO module.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 Appollo Pro
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
#include "ocvRealsenseWrapper.h"
#include "static.h"
#include <Windows.h>
#include <nuiOpenCl.h>

#ifdef ALLOW_BENCHMARKING	
#include "nuiBenchmark.h"
#endif

NUI_DATAPACKET_DEFAULT_DECLARATION(RealSenseModule, IplImage*)

class nuiRealSenseModule : public nuiModule {
public:
	nuiRealSenseModule();
	virtual ~nuiRealSenseModule();

	void update();
	void start();

private:
	nuiEndpoint * output;
	nuiDataPacket* outputDataPacket;
	void initAnchorPoints();

	//cv::Mat frame;
	IplImage* img;

	ocvRealsenseWrapper realsenseW;
#ifdef ALLOW_BENCHMARKING	
	nuiBenchmark benchmark;
#endif

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

START_EXPORT_MODULES(RealSenseModule)
setFrameworkPtr(params->frameworkPointer);
REGISTER_MODULE(nuiRealSenseModule, "nuiRealSenseModule", 1, 0)
END_EXPORT_MODULES()

#endif
