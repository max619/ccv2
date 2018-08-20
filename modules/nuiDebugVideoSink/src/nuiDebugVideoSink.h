/**
 * \file      nuiDebugVideoSink.h
 *            Sink to display video
 * \author    Maxim Bagryantsev
 * \date      2012-2018
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 * \copyright Copyright 2018 Appollo Pro. All rights reserved.
 */

#ifndef NUI_DEBUG_VIDEO_SINK_H
#define NUI_DEBUG_VIDEO_SINK_H


#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <nuiTrackerStructs.h>
#include <Windows.h>
#include <string.h>

#ifdef ALLOW_BENCHMARKING	
#include "nuiBenchmark.h"
#endif

class nuiDebugVideoSink : public nuiModule {
public:
	nuiDebugVideoSink();
	virtual ~nuiDebugVideoSink();

	void update();
	void start();
	void stop();

protected:
	void onSetupThread();
	void onExitThread();

private:
	void drawBlob(Blob& b, IplImage* img);
	nuiEndpoint * input;

	CvFont font;

	IplImage *dispFrame;

#ifdef ALLOW_BENCHMARKING	
	nuiBenchmark benchmark;
#endif
	double hScale = 0.5;
	double vScale = 0.5;
	int lineWidth = 1;
	bool wndInit;
	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiDebugVideoSink)
IMPLEMENT_DEALLOCATOR(nuiDebugVideoSink)

START_IMPLEMENT_DESCRIPTOR(nuiDebugVideoSink, "native", "Print streams/structures to the debug console in text format")
descriptor->setInputEndpointsCount(1);
descriptor->setOutputEndpointsCount(0);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("*");
descriptor->addInputEndpointDescriptor(inputDescriptor, 0);
descriptor->property("use_thread").set(true);
END_IMPLEMENT_DESCRIPTOR(nuiDebugVideoSink)

START_EXPORT_MODULES(RealSenseModule)
REGISTER_MODULE(nuiDebugVideoSink, "nuiDebugVideoSink", 1, 0)
END_EXPORT_MODULES()

#endif

