
/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiExampleModule.cpp
// Purpose:     Example Module to be used for developments.
// Author:      Christian Moore
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////
#include "nuiRealSenseModule.h"

nuiRealSenseModuleDataPacket::~nuiRealSenseModuleDataPacket()
{
	cvReleaseImage(&data);
};

nuiDataPacketError nuiRealSenseModuleDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiRealSenseModuleDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiRealSenseModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiRealSenseModuleDataPacket* newDataPacket = new nuiRealSenseModuleDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = cvCloneImage(this->data);

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiRealSenseModuleDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(RealSenseModule, "native", "Input module to grab video from webcameras.");

nuiRealSenseModule::nuiRealSenseModule() : nuiModule() {
	MODULE_INIT();

	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiRealSenseModuleDataPacket();
}

nuiRealSenseModule::~nuiRealSenseModule() {
	
}

void nuiRealSenseModule::update() {
	this->output->lock();
	this->output->clear();

	this->outputDataPacket->packData(img);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
}

void nuiRealSenseModule::start() {

	LOG(NUI_DEBUG, "Starting nuiWebcamModule");
	nuiModule::start();
}

