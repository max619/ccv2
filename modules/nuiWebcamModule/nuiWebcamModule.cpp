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
