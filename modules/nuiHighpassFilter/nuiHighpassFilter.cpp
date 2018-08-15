/**
* \file      nuiHighpassFilter.cpp
*            Highpass filter
* \author    Scott Halstvedt
* \author    Maxim Bagryantsev
* \date      2012-2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiHighpassFilter.h"

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(HighpassFilter)

MODULE_DECLARE(HighpassFilter, "native", "Filter gaussian");

nuiHighpassFilter::nuiHighpassFilter() : nuiModule() {
	MODULE_INIT();

	this->input = new nuiEndpoint(this);
	this->input->setTypeDescriptor(std::string("IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->input);
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiHighpassFilterDataPacket();
}

nuiHighpassFilter::~nuiHighpassFilter() {
}

void nuiHighpassFilter::update() {
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if (packet == NULL) return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	filterFrame = cvCloneImage(frame);

	CvMat* kernel = 0;
	IplImage* dst = cvCloneImage(frame);

	kernel = cvCreateMat(9, 9, CV_32FC1);
	cvSet2D(kernel, 0, 0, cvRealScalar(0));
	cvSet2D(kernel, 0, 1, cvRealScalar(-1));
	cvSet2D(kernel, 0, 2, cvRealScalar(0));
	cvSet2D(kernel, 1, 0, cvRealScalar(-1));
	cvSet2D(kernel, 1, 1, cvRealScalar(5));
	cvSet2D(kernel, 1, 2, cvRealScalar(-1));
	cvSet2D(kernel, 2, 0, cvRealScalar(0));
	cvSet2D(kernel, 2, 1, cvRealScalar(-1));
	cvSet2D(kernel, 2, 2, cvRealScalar(0));

	cvFilter2D(filterFrame, dst, kernel, cvPoint(-1, -1));
	IplImage* oldImage = cvCloneImage(dst);
	this->outputDataPacket->packData(oldImage);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
	cvReleaseImage(&filterFrame);
	cvReleaseImage(&dst);
}

void nuiHighpassFilter::start() {
	nuiModule::start();
	LOG(NUI_DEBUG, "starting highpass filter");
}

void nuiHighpassFilter::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}