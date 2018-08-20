/**
* \file      buiBlobTrackerModule.cpp
* \author    Anatoly Lushnikov
* \author    Maxim Bagryantsev
* \date      2012-2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiBlobTracker.h"

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_BLOBVECTOR(BlobTracker)

MODULE_DECLARE(BlobTrackerModule, "native", "Track found contours/blobs")

nuiBlobTrackerModule::nuiBlobTrackerModule() : nuiModule()
{
	MODULE_INIT();
	this->_pInput = new nuiEndpoint(this);
	this->_pInput->setTypeDescriptor(std::string("IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->_pInput);

	this->_pOutput = new nuiEndpoint(this);
	this->_pOutput->setTypeDescriptor(std::string("BlobVector"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->_pOutput);

	this->_pOutputDataPacket = new nuiBlobTrackerDataPacket();

	bTracker = new blobTracker();

	LinkPropertyAndSetDefaultVal("blobColor", NUI_PROPERTY_INTEGER, p.blobColor, 255);
	LinkPropertyAndSetDefaultVal("filterByArea", NUI_PROPERTY_BOOL, p.filterByArea, true);
	LinkPropertyAndSetDefaultVal("filterByColor", NUI_PROPERTY_BOOL, p.filterByColor, true);
	LinkPropertyAndSetDefaultVal("filterByCircularity", NUI_PROPERTY_BOOL, p.filterByCircularity, false);
	LinkPropertyAndSetDefaultVal("filterByInertia", NUI_PROPERTY_BOOL, p.filterByInertia, false);
	LinkPropertyAndSetDefaultVal("filterByConvexity", NUI_PROPERTY_BOOL, p.filterByConvexity, false);

	LinkPropertyAndSetDefaultVal("maxArea", NUI_PROPERTY_FLOAT, p.maxArea, 5000.f);
	LinkPropertyAndSetDefaultVal("maxCircularity", NUI_PROPERTY_FLOAT, p.maxCircularity, 3.4028E+38f);
	LinkPropertyAndSetDefaultVal("maxConvexity", NUI_PROPERTY_FLOAT, p.maxConvexity, 3.4028E+38f);
	LinkPropertyAndSetDefaultVal("maxInertiaRatio", NUI_PROPERTY_FLOAT, p.maxInertiaRatio, 3.4028E+38f);
	LinkPropertyAndSetDefaultVal("maxThreshold", NUI_PROPERTY_FLOAT, p.maxThreshold, 220.f);
	LinkPropertyAndSetDefaultVal("minArea", NUI_PROPERTY_FLOAT, p.minArea, 25.f);
	LinkPropertyAndSetDefaultVal("minCircularity", NUI_PROPERTY_FLOAT, p.minCircularity, 0.8f);
	LinkPropertyAndSetDefaultVal("minConvexity", NUI_PROPERTY_FLOAT, p.minConvexity, 0.949f);
	LinkPropertyAndSetDefaultVal("minDistBetweenBlobs", NUI_PROPERTY_FLOAT, p.minDistBetweenBlobs, 10.f);
	LinkPropertyAndSetDefaultVal("minInertiaRatio", NUI_PROPERTY_FLOAT, p.minInertiaRatio, 0.1f);
	LinkPropertyAndSetDefaultVal("minThreshold", NUI_PROPERTY_FLOAT, p.minThreshold, 50.f);
	LinkPropertyAndSetDefaultVal("thresholdStep", NUI_PROPERTY_FLOAT, p.thresholdStep, 10.f);
	
	LinkPropertyAndSetDefaultVal("maxDistance", NUI_PROPERTY_FLOAT, maxdist, 20.f);

	LinkPropertyAndSetDefaultVal("blob_life", NUI_PROPERTY_INTEGER, bTracker->blobLive, 5);
	LinkPropertyAndSetDefaultVal("min_blob_life", NUI_PROPERTY_INTEGER, bTracker->minblobLive, 3);
	LinkPropertyAndSetDefaultVal("apply_ease_func", NUI_PROPERTY_BOOL, bTracker->applyEasingFunction, true);
	LinkPropertyAndSetDefaultVal("ease_func_coeff", NUI_PROPERTY_FLOAT, bTracker->beginCoeff, 0.01f);
	p.minRepeatability = 2;
	bDetector = new blobDetector(p);

	bTracker->setMaxDistance(maxdist);
};

nuiBlobTrackerModule::~nuiBlobTrackerModule()
{

};

void nuiBlobTrackerModule::update()
{
#ifdef ALLOW_BENCHMARKING	
	benchmark.startBencmarking();
#endif
	this->_pOutput->lock();
	this->_pOutput->clear();

	void* data;
	nuiDataPacket* packet = this->_pInput->getData();
	if (packet == NULL)
		return;
	packet->unpackData(data);
	if (data == NULL)
		return;

	IplImage* img = (IplImage*)data;
	std::vector<Blob> blobs = bDetector->detectBlobs(img);
	BlobVector* trackedblobsPtr = bTracker->trackBlobs(blobs);
	trackedblobsPtr->targetResolution.width = img->width;
	trackedblobsPtr->targetResolution.height = img->height;

	this->_pOutputDataPacket->packData(trackedblobsPtr);
	this->_pOutput->setData(this->_pOutputDataPacket);
	this->_pOutput->transmitData();
	this->_pOutput->unlock();

	if (packet->isLocalCopy())
		delete packet;

	delete trackedblobsPtr;

#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("nuiBlobTrackerModule::update blob detecting");
#endif
};

void nuiBlobTrackerModule::start()
{
	nuiModule::start();
};

void nuiBlobTrackerModule::stop()
{
	nuiModule::stop();
};

void nuiBlobTrackerModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{
	if (name == "maxDistance")
	{
		bTracker->setMaxDistance(prop->asDouble());
	}
	else
	{
	}
}