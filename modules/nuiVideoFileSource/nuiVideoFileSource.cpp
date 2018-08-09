#include "nuiVideoFileSource.h"



NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(VideoFileSource)

MODULE_DECLARE(VideoFileSource, "native", "Get video from a file source")

nuiVideoFileSource::nuiVideoFileSource() : nuiModule()
{
	MODULE_INIT();

	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiVideoFileSourceDataPacket();
};

nuiVideoFileSource::~nuiVideoFileSource()
{};

void nuiVideoFileSource::update()
{
	this->output->lock();

	this->output->clear();

	frame = cvQueryFrame(cap);
	if (frame == NULL) {
		cvReleaseCapture(&cap);
		cvReleaseImage(&frame);
		std::string path = this->property("path").asString();
		cap = cvCaptureFromAVI(path.c_str());
		frame = cvQueryFrame(cap);
	}
	CvFont font;
	double hScale = 0.5;
	double vScale = 0.5;
	int    lineWidth = 1;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);
	std::ostringstream oss;
	oss << "~ " << this->timer->getAverageFPS() << " FPS";
	cvRectangle(frame, cvPoint(150, 0), cvPoint(300, 20), cvScalar(0, 0, 0), CV_FILLED, CV_AA);
	cvRectangle(frame, cvPoint(150, 0), cvPoint(300, 20), cvScalar(255, 100, 100), 2, CV_AA);
	cvPutText(frame, oss.str().c_str(), cvPoint(155, 15), &font, cvScalar(255, 255, 255));

	this->outputDataPacket->packData((void*)frame);
	this->output->setData(this->outputDataPacket);
	this->output->transmitData();
	this->output->unlock();
};

void nuiVideoFileSource::start()
{
	std::string path = this->property("path").asString();
	cap = cvCaptureFromAVI(path.c_str());
	nuiModule::start();
};

void nuiVideoFileSource::stop()
{
	cvReleaseCapture(&cap);
	//cvReleaseImage(&frame);
	nuiModule::stop();
};

void nuiVideoFileSource::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}