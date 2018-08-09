#include "nuiWebcamModule.h"
#include "DeviceEnumerator.h"



NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(WebcamModule)

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
	LinkPropertyAndSetDefaultVal("camid", NUI_PROPERTY_INTEGER, camid, 0);
}

nuiWebcamModule::~nuiWebcamModule() {
	delete capture;
}

void nuiWebcamModule::update() {
	this->output->lock();
	this->output->clear();

	if (capture == NULL)
		return;
	mutex.lock();
	if (capture->read(frame))
	{
		if (img == NULL)
			img = new IplImage(frame);
		this->outputDataPacket->packData(img);
		this->output->setData(this->outputDataPacket);
		this->output->transmitData();		
	}
	mutex.unlock();
	this->output->unlock();
}

void nuiWebcamModule::start() {
	if (!capture->isOpened())
		capture->open(camid);
	LOG(NUI_DEBUG, "Starting nuiWebcamModule");
	nuiModule::start();
}

void nuiWebcamModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{
	if (name == "camid")
	{
		mutex.lock();
		delete capture;
		capture = new cv::VideoCapture();
		capture->open(camid);
		mutex.unlock();
	}
}