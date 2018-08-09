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
	this->setOutputEndpointCount(2);
	this->setOutputEndpoint(0, this->frameOutput);
	this->setOutputEndpoint(1, this->bgOutput);

	this->outputDataPacket = new nuiBackgroundSubtractModuleDataPacket();
	LinkPropertyAndSetDefaultVal("history", NUI_PROPERTY_INTEGER, history, 500);
	LinkPropertyAndSetDefaultVal("threshold", NUI_PROPERTY_DOUBLE, threshold, 16.);
	LinkPropertyAndSetDefaultVal("detectshadows", NUI_PROPERTY_BOOL, detectshadows, false);
	this->bg = cv::createBackgroundSubtractorMOG2(history, threshold, detectshadows);

}

nuiBackgroundSubtractModule::~nuiBackgroundSubtractModule() {
	bg.release();
}

void nuiBackgroundSubtractModule::update() {
	this->frameOutput->lock();
	this->frameOutput->clear();
	this->bgOutput->lock();
	this->bgOutput->clear();
	this->input->lock();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if (packet == NULL) return;
	packet->unpackData(data);
	if (data == NULL) return;
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);
	cv::Mat matFrame = cv::cvarrToMat(filterFrame, true);
	cv::Mat foreFrame;
	bg->apply(matFrame, foreFrame);
	//this->outputDataPacket = new 
	if (this->outputDataPacket->packData(new IplImage(foreFrame)) == NUI_DATAPACKET_OK)
	{
		this->frameOutput->setData(this->outputDataPacket);
		this->frameOutput->transmitData();
	}
	this->frameOutput->unlock();
	this->input->unlock();
	cvReleaseImage(&filterFrame);
	if (packet->isLocalCopy())
	{
		nuiReleaseDataPacket(&packet);
	}
}

void nuiBackgroundSubtractModule::start() {
	nuiModule::start();
	LOG(NUI_DEBUG, "starting gaussian filter");
}

void nuiBackgroundSubtractModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
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