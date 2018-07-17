/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiExampleModule.cpp
// Purpose:     Example Module to be used for developments.
// Author:      Christian Moore
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiExampleModule.h"

nuiExampleModuleDataPacket::~nuiExampleModuleDataPacket()
{
		cvReleaseImage(&data);
};

nuiDataPacketError nuiExampleModuleDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiExampleModuleDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiExampleModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiExampleModuleDataPacket* newDataPacket = new nuiExampleModuleDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = cvCloneImage(this->data);

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiExampleModuleDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(ExampleModule, "native", "Example Module to be used for developments.");

nuiExampleModule::nuiExampleModule() : nuiModule() {
    MODULE_INIT();

	this->input = new nuiEndpoint(this);
    this->input->setTypeDescriptor(std::string("IplImage"));
    this->setInputEndpointCount(1);
    this->setInputEndpoint(0,this->input);
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0,this->output);

	this->outputDataPacket = new nuiExampleModuleDataPacket();
}

nuiExampleModule::~nuiExampleModule() {
}

void nuiExampleModule::update() { 
	this->output->lock();
	this->output->clear();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if(packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);
	cv::Mat newFrame = cv::cvarrToMat(filterFrame);
	cv::Mat edges = cv::cvarrToMat(filterFrame);
	if(!this->property("disable").asBool()) cv::Canny(edges, edges, 0, 30, 3);
	IplImage* oldImage = new IplImage(edges);
	this->outputDataPacket->packData(oldImage);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
	cvReleaseImage(&filterFrame);

	if (packet->isLocalCopy())
	{
		nuiReleaseDataPacket(&packet);
	}
}

void nuiExampleModule::start() {
	nuiModule::start();
	this->timer->Start();
	LOG(NUI_DEBUG,"Starting nuiExampleModule");
}