/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiExampleModule.h
// Purpose:     Example Module to be used for developments.
// Author:      Christian Moore
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////


#ifndef NUI_WEBCAM_MODULE_H
#define NUI_WEBCAM_MODULE_H

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

NUI_DATAPACKET_DEFAULT_IMPLEMENTATION(WebcamModule, IplImage*)

class nuiWebcamModule : public nuiModule {
public:
	nuiWebcamModule();
	virtual ~nuiWebcamModule();

	void update();
	void start();

private:
	cv::VideoCapture * capture;
	nuiEndpoint* output;
	nuiDataPacket* outputDataPacket;
	void initCapture();

	cv::Mat frame;
	IplImage* img;

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiWebcamModule)
IMPLEMENT_DEALLOCATOR(nuiWebcamModule)

START_IMPLEMENT_DESCRIPTOR(nuiWebcamModule, "native", "Get video data from webcam")
descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addOutputEndpointDescriptor(outputDescriptor, 0);
//descriptor->property("use_thread").set(true);
//descriptor->property("oscillator_mode").set(true);
//descriptor->property("oscillator_wait").set(50);
END_IMPLEMENT_DESCRIPTOR(nuiWebcamModule)

START_MODULE_EXIT()
END_MODULE_EXIT()

START_MODULE_REGISTRATION()
REGISTER_PLUGIN(nuiWebcamModule, "nuiWebcamModule", 1, 0)
END_MODULE_REGISTRATION()

#endif

