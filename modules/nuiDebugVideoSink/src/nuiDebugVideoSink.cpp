#include "nuiDebugVideoSink.h"
#include <sstream>

MODULE_DECLARE(DebugVideoSink, "native", "Sink to display video");

nuiDebugVideoSink::nuiDebugVideoSink() : nuiModule() {
	MODULE_INIT();

	this->input = new nuiEndpoint(this);
	this->input->setTypeDescriptor(std::string("IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->input);
	dispFrame = NULL;
}

nuiDebugVideoSink::~nuiDebugVideoSink()
{
	cvReleaseImage(&dispFrame);
}

void nuiDebugVideoSink::update()
{
	this->input->lock();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if (packet == NULL)
		return; 
#ifdef ALLOW_BENCHMARKING	
	benchmark.startBencmarking();
#endif
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	if (dispFrame == NULL)
		dispFrame = cvCloneImage(frame);
	else
		cvCopy(frame, dispFrame);
	CvFont font;
	double hScale = 0.5;
	double vScale = 0.5;
	int lineWidth = 1;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);
	std::ostringstream oss;
	oss << "~ " << this->timer->getAverageFPS() << " FPS";
	cvRectangle(dispFrame, cvPoint(0, 0), cvPoint(150, 20), cvScalar(0, 0, 0), CV_FILLED, CV_AA);
	cvRectangle(dispFrame, cvPoint(0, 0), cvPoint(150, 20), cvScalar(255, 255, 255), 2, CV_AA);
	cvPutText(dispFrame, oss.str().c_str(), cvPoint(5, 15), &font, cvScalar(255, 255, 255));
	cvShowImage((this->property("id")).asString().c_str(), dispFrame);
	cvWaitKey(1);
#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("nuiDebugVideoSink::update frame rendering");
#endif
	delete packet;
	this->input->unlock();


}

void nuiDebugVideoSink::start()
{
	nuiModule::start();
	LOG(NUI_DEBUG, "starting video sink");
}