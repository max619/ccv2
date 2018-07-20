
/////////////////////////////////////////////////////////////////////////////
// Name:        modules/nuiExampleModule.cpp
// Purpose:     Example Module to be used for developments.
// Author:      Christian Moore
// Copyright:   (c) 2012 NUI Group
/////////////////////////////////////////////////////////////////////////////
#include "nuiRealSenseModule.h"

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(RealSenseModule)

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

