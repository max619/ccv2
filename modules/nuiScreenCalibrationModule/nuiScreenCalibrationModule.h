/**
* \file      nuiScreenCalibrationModule.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef NUI_SCREENCALIBRATION_MODULE
#define NUI_SCREENCALIBRATION_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include <nuiTrackerStructs.h>
#include <opencv2/imgproc.hpp>
#ifdef ALLOW_BENCHMARKING	
#include "nuiBenchmark.h"
#endif

NUI_DATAPACKET_DEFAULT_DECLARATION(ScreenCalibrationModuleIplImage, void*)
NUI_DATAPACKET_DEFAULT_DECLARATION(ScreenCalibrationModuleBlobVector, void*)

class nuiScreenCalibrationModule : public nuiModule
{
public:
	nuiScreenCalibrationModule();
	~nuiScreenCalibrationModule();

	void update();
	void start();
	void stop();

protected:
private:
	CvSize destsize;
	CvPoint2D32f screenpoints[4];
	CvPoint2D32f dstpoints[4];
	CvMat* homography;

	nuiDataPacket*	_pOutputDataPacket0;
	nuiDataPacket*	_pOutputDataPacket1;
	nuiEndpoint*	_pInput;
	nuiEndpoint*	_pOutput0;
#ifdef ALLOW_BENCHMARKING
	nuiBenchmark benchmark;
#endif

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiScreenCalibrationModule)
IMPLEMENT_DEALLOCATOR(nuiScreenCalibrationModule)

START_IMPLEMENT_DESCRIPTOR(nuiScreenCalibrationModule, "native", "Calibrate blobs/IplImage to screen size")

descriptor->setInputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("BlobVector,IplImage");
descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

descriptor->setOutputEndpointsCount(1);
nuiEndpointDescriptor* outputDescriptor0 = new nuiEndpointDescriptor("BlobVector,IplImage");
descriptor->addInputEndpointDescriptor(outputDescriptor0, 0);

END_IMPLEMENT_DESCRIPTOR(nuiScreenCalibrationModule)


START_EXPORT_MODULES(ScreenCalibrationModule)
REGISTER_MODULE(nuiScreenCalibrationModule, "nuiScreenCalibrationModule", 1, 0)
END_EXPORT_MODULES()

#endif
