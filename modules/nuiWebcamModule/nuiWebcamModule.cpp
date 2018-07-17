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
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiWebcamModuleDataPacket();
}

nuiWebcamModule::~nuiWebcamModule() {
	cvReleaseCapture(&capture);
	cvReleaseImage(&frame);
}

void nuiWebcamModule::update() {
	this->output->lock();
	this->output->clear();

	frame = cvQueryFrame(capture);
	if (frame == NULL) {
		cvReleaseCapture(&capture);
		cvReleaseImage(&frame);
		initCapture();
		frame = cvQueryFrame(capture);
	}


	this->outputDataPacket->packData(frame);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
	//cvReleaseImage(&frame);
	//delete packet;
	cvReleaseImage(&frame);
}

void nuiWebcamModule::start() {
	
	initCapture();
	
	LOG(NUI_DEBUG, "Starting nuiWebcamModule");
}

void nuiWebcamModule::initCapture()
{
	/*DeviceEnumerator enumer = DeviceEnumerator();
	auto map = enumer.getVideoDevicesMap();*/
	int camid = this->property("camid").asInteger();
	capture = cvCaptureFromCAM(camid + CV_CAP_ANY);
}
