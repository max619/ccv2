#include "nuiExampleModule.h"

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(ExampleModule)

MODULE_DECLARE(ExampleModule, "native", "Example Module to be used for developments.");

nuiExampleModule::nuiExampleModule() : nuiModule() {
	MODULE_INIT();

	this->input = new nuiEndpoint(this);
	this->input->setTypeDescriptor(std::string("IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->input);
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiExampleModuleDataPacket();
}

nuiExampleModule::~nuiExampleModule() {
}

void nuiExampleModule::update() {
	this->output->lock();
	this->output->clear();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if (packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);
	cv::Mat newFrame = cv::cvarrToMat(filterFrame);
	cv::Mat edges = cv::cvarrToMat(filterFrame);
	if (!this->property("disable").asBool()) cv::Canny(edges, edges, 0, 30, 3);
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
	LOG(NUI_DEBUG, "Starting nuiExampleModule");
}