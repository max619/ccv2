/**
* \file      buiBlobTrackerModule.h
* \author    Anatoly Lushnikov
* \author    Maxim Bagryantsev
* \date      2012-2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/


#ifndef NUI_BLOBTRACKER_MODULE
#define NUI_BLOBTRACKER_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "blobDetector.h"
#include "blobTracker.h"
#include <nuiTrackerStructs.h>
#ifdef ALLOW_BENCHMARKING	
#include "nuiBenchmark.h"
#endif
NUI_DATAPACKET_DEFAULT_DECLARATION(BlobTracker, void*)

class nuiBlobTrackerModule : public nuiModule
{
public:
	nuiBlobTrackerModule();
	~nuiBlobTrackerModule();

	void update();
	void start();
	void stop();

protected:
private:
	blobTracker*	bTracker;
	blobDetector*	bDetector;

	nuiDataPacket*	_pOutputDataPacket;
	nuiEndpoint*	_pInput;
	nuiEndpoint*	_pOutput; 

	double maxdist;
	cv::SimpleBlobDetector::Params p;
#ifdef ALLOW_BENCHMARKING
	nuiBenchmark benchmark;
#endif

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiBlobTrackerModule)
IMPLEMENT_DEALLOCATOR(nuiBlobTrackerModule)

START_IMPLEMENT_DESCRIPTOR(nuiBlobTrackerModule, "native", "Track found contours/blobs")

descriptor->setInputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor = new nuiEndpointDescriptor("BlobVector");
descriptor->addInputEndpointDescriptor(outputDescriptor, 0);

END_IMPLEMENT_DESCRIPTOR(nuiBlobTrackerModule)


START_EXPORT_MODULES(BlobTrackerModule)
REGISTER_MODULE(nuiBlobTrackerModule, "nuiBlobTrackerModule", 1, 0)
END_EXPORT_MODULES()

#endif