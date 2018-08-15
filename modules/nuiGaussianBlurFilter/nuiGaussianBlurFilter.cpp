#include "nuiGaussianBlurFilter.h"

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(GaussianBlurFilter)

MODULE_DECLARE(GaussianBlurFilter, "native", "Filter gaussian");

nuiGaussianBlurFilter::nuiGaussianBlurFilter() : nuiModule() {
	MODULE_INIT();

	this->input = new nuiEndpoint(this);
	this->input->setTypeDescriptor(std::string("IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->input);
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiGaussianBlurFilterDataPacket();

	LinkPropertyAndSetDefaultVal("blur_radius_x", NUI_PROPERTY_INTEGER, blursize.width, 5);
	LinkPropertyAndSetDefaultVal("blur_radius_y", NUI_PROPERTY_INTEGER, blursize.height, 5);
}

nuiGaussianBlurFilter::~nuiGaussianBlurFilter() {
}

void nuiGaussianBlurFilter::update() {
	this->output->lock();
	this->output->clear();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if (packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	cv::Mat newFrame = cv::cvarrToMat(frame);
	cv::GaussianBlur(newFrame, newFrame, blursize, 4, 4);
	this->outputDataPacket->packData(frame);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
	/*newFrame.release();
	blur.release();*/
	if (packet->isLocalCopy())
		delete packet;
}

void nuiGaussianBlurFilter::start() {
	nuiModule::start();
	dev = false;
	LOG(NUI_DEBUG, "starting gaussian filter");
}

void nuiGaussianBlurFilter::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}