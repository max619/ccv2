/**
* \file      nuiPluginManager.cpp
*            Filter to detect edges
* \author    Scott Halstvedt
* \author    Maxim Bagryantsev
* \date      2012-2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiBackgroundSubtractModule.h"

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(BackgroundSubtractModule)

MODULE_DECLARE(BackgroundSubtractModule, "native", "Filter out background");

nuiBackgroundSubtractModule::nuiBackgroundSubtractModule() : nuiModule() {
	MODULE_INIT();
	this->input = new nuiEndpoint(this);
	this->input->setTypeDescriptor(std::string("IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->input);
	this->frameOutput = new nuiEndpoint(this);
	this->frameOutput->setTypeDescriptor(std::string("IplImage"));
	this->bgOutput = new nuiEndpoint(this);
	this->bgOutput->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->frameOutput);
	this->filterFrame = NULL;

	this->outputDataPacket = new nuiBackgroundSubtractModuleDataPacket();
	LinkPropertyAndSetDefaultVal("history", NUI_PROPERTY_INTEGER, history, 500);
	LinkPropertyAndSetDefaultVal("threshold", NUI_PROPERTY_DOUBLE, threshold, 16.);
	LinkPropertyAndSetDefaultVal("detectshadows", NUI_PROPERTY_BOOL, detectshadows, false);
	this->bg = cv::createBackgroundSubtractorMOG2(history, threshold, detectshadows);

}

nuiBackgroundSubtractModule::~nuiBackgroundSubtractModule() {
	bg.release();
	if (filterFrame != NULL)
	{
		foreFrame.release();
		cvReleaseImage(&filterFrame);
	}
}

void nuiBackgroundSubtractModule::update() {
	this->frameOutput->lock();
	this->frameOutput->clear();
	
	this->input->lock();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if (packet == NULL) return;
	packet->unpackData(data);
	if (data == NULL) return;
	IplImage* frame = (IplImage*)data;
	cv::Mat matFrame = cv::cvarrToMat(frame);
	if (filterFrame == NULL)
	{
		filterFrame = cvCreateImage(CvSize(frame->width, frame->height), IPL_DEPTH_8U, 1);
		foreFrame = cv::cvarrToMat(filterFrame);
	}

	bg->apply(matFrame, foreFrame);
	//this->outputDataPacket = new 
	if (this->outputDataPacket->packData(filterFrame) == NUI_DATAPACKET_OK)
	{
		this->frameOutput->setData(this->outputDataPacket);
		this->frameOutput->transmitData();
	}
	matFrame.release();
	this->frameOutput->unlock();
	this->input->unlock();
	if (packet->isLocalCopy())
	{
		delete packet;
	}
}

void nuiBackgroundSubtractModule::start() {
	nuiModule::start();
	LOG(NUI_DEBUG, "starting gaussian filter");
}

void nuiBackgroundSubtractModule::stop()
{
	if (filterFrame != NULL)
	{
		foreFrame.release();
		cvReleaseImage(&filterFrame);
	}
	nuiModule::stop();
}

void nuiBackgroundSubtractModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{
	if (bg != NULL)
	{
		if (name == "history")
		{
			bg->setHistory(history);
		}
		else if (name == "threshold")
		{
			bg->setVarThreshold(threshold);
		}
		else if (name == "detectshadows")
		{
			bg->setDetectShadows(detectshadows);
		}
	}
}