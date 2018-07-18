/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiExampleModule.cpp
// Purpose:     Example Module to be used for developments.
// Author:      Christian Moore
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////

#include "nuiWebcamModule.h"
#include "DeviceEnumerator.h"

nuiWebcamModuleDataPacket::~nuiWebcamModuleDataPacket()
{
	cvReleaseImage(&data);
};

nuiDataPacketError nuiWebcamModuleDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (IplImage*)_data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacketError nuiWebcamModuleDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return NUI_DATAPACKET_OK;
};

nuiDataPacket* nuiWebcamModuleDataPacket::copyPacketData(nuiDataPacketError &errorCode)
{
	nuiWebcamModuleDataPacket* newDataPacket = new nuiWebcamModuleDataPacket();

	//! TODO : Test if this implies deep copy
	IplImage* newData = cvCloneImage(this->data);

	newDataPacket->packData(newData);
	newDataPacket->setLocalCopy(true);

	errorCode = NUI_DATAPACKET_OK;
	return newDataPacket;
};

char* nuiWebcamModuleDataPacket::getDataPacketType()
{
	return "IplImage";
};

MODULE_DECLARE(WebcamModule, "native", "Input module to grab video from webcameras.");

nuiWebcamModule::nuiWebcamModule() : nuiModule() {
	MODULE_INIT();

	capture = new cv::VideoCapture();
	img = NULL;


	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiWebcamModuleDataPacket();
}

nuiWebcamModule::~nuiWebcamModule() {
	delete capture;
}

void nuiWebcamModule::update() {
	this->output->lock();
	this->output->clear();

	if (capture == NULL)
		return;
	
	if (capture->read(frame))
	{
		if (img == NULL)
			img = new IplImage(frame);
		this->outputDataPacket->packData(img);
		this->output->setData(this->outputDataPacket);
		this->output->transmitData();		
	}
	this->output->unlock();
}

void nuiWebcamModule::start() {
	
	initCapture();

	LOG(NUI_DEBUG, "Starting nuiWebcamModule");
	nuiModule::start();
}

void nuiWebcamModule::initCapture()
{
	/*DeviceEnumerator enumer = DeviceEnumerator();
	auto map = enumer.getVideoDevicesMap();*/
	int camid = this->hasProperty("camid") ? this->property("camid").asInteger() : 0;
	capture->open(camid);
}
