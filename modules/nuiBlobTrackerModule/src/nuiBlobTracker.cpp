//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiBlobTracker
// Purpose:		Track found contours/blobs
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

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

	bTracker = NULL;
	bDetector = NULL;
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
	if(packet == NULL) 
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

	releaseBlobVector(trackedblobsPtr);
	
#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("nuiBlobTrackerModule::update blob detecting");
#endif
};

void nuiBlobTrackerModule::start()
{
	cv::SimpleBlobDetector::Params p;
	p.blobColor = (uchar)this->hasProperty("blobColor") ? this->property("blobColor").asInteger() : 255;
	p.filterByArea = this->hasProperty("filterByArea") ? this->property("filterByArea").asBool() : true;
	p.filterByCircularity = this->hasProperty("filterByCircularity") ? this->property("filterByCircularity").asBool() : false;
	p.filterByColor = this->hasProperty("filterByColor") ? this->property("filterByColor").asBool() : true;
	p.filterByConvexity = this->hasProperty("filterByConvexity") ? this->property("filterByConvexity").asBool() : false;
	p.filterByInertia = this->hasProperty("filterByInertia") ? this->property("filterByInertia").asBool() : false;

	p.maxArea = static_cast<float>(this->hasProperty("maxArea") ? this->property("maxArea").asDouble() : 5000.);
	p.maxCircularity = static_cast<float>(this->hasProperty("maxCircularity") ? this->property("maxCircularity").asDouble() : 3.4028E+38f);
	p.maxConvexity = static_cast<float>(this->hasProperty("maxConvexity") ? this->property("maxConvexity").asDouble() : 3.4028E+38f);
	p.maxInertiaRatio = static_cast<float>(this->hasProperty("maxInertiaRatio") ? this->property("maxInertiaRatio").asDouble() : 3.4028E+38f);
	p.maxThreshold = static_cast<float>(this->hasProperty("maxThreshold") ? this->property("maxThreshold").asDouble() : 220.f);
	p.minArea = static_cast<float>(this->hasProperty("minArea") ? this->property("minArea").asDouble() : 25.f);
	p.minCircularity = static_cast<float>(this->hasProperty("minCircularity") ? this->property("minCircularity").asDouble() : 0.8f);
	p.minConvexity = static_cast<float>(this->hasProperty("minConvexity") ? this->property("minConvexity").asDouble() : 0.949f);
	p.minDistBetweenBlobs = static_cast<float>(this->hasProperty("minDistBetweenBlobs") ? this->property("minDistBetweenBlobs").asDouble() : 10.);
	p.minInertiaRatio = static_cast<float>(this->hasProperty("minInertiaRatio") ? this->property("minInertiaRatio").asDouble() : 0.1);
	p.minRepeatability = static_cast<float>(this->hasProperty("minRepeatability") ? this->property("minRepeatability").asDouble() : 2);
	p.minThreshold = static_cast<float>(this->hasProperty("minThreshold") ? this->property("minThreshold").asDouble() : 50);
	p.thresholdStep = static_cast<float>(this->hasProperty("thresholdStep") ? this->property("thresholdStep").asDouble() : 10.);

	bDetector = new blobDetector(p);

	bTracker = new blobTracker();
	bTracker->setMaxDistance(this->hasProperty("maxDistance") ? this->property("maxDistance").asDouble() : 20.);
	nuiModule::start();
};

void nuiBlobTrackerModule::stop()
{
	nuiModule::stop();
};
