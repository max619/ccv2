/**
* \file      nuiDebugVideoSink.cpp
*            Sink to display video
* \author    Maxim Bagryantsev
* \date      2012-2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiDebugVideoSink.h"
#include <sstream>

MODULE_DECLARE(DebugVideoSink, "native", "Sink to display video");

nuiDebugVideoSink::nuiDebugVideoSink() : nuiModule() {
	MODULE_INIT();

	this->input = new nuiEndpoint(this);
	this->input->setTypeDescriptor(std::string("*"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->input);
	dispFrame = NULL;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);
	wndInit = false;
}

nuiDebugVideoSink::~nuiDebugVideoSink()
{
	cvReleaseImage(&dispFrame);
}

void nuiDebugVideoSink::update()
{
#ifdef ALLOW_BENCHMARKING	
	benchmark.startBencmarking();
#endif
	this->input->lock();
	void* data = NULL;
	nuiDataPacket* packet = this->input->getData(); 
	if (packet == NULL)
	{
		this->input->unlock();
		return;
	}
	packet->unpackData(data);
	if (data == NULL)
	{
		this->input->unlock();
		return;
	}
	
	if (strcmp( packet->getDataPacketType(), "IplImage") == 0)
	{
		IplImage* frame = (IplImage*)data;
		if (dispFrame == NULL)
		{
			dispFrame = cvCloneImage(frame);
		}
		else
			cvCopy(frame, dispFrame);
	}
	else if(strcmp(packet->getDataPacketType(), "BlobVector") == 0)
	{
		BlobVector* blobs = (BlobVector*)data;

		if (dispFrame == NULL)
			dispFrame = cvCreateImage(blobs->targetResolution, IPL_DEPTH_8U, 1);

		ZeroMemory(dispFrame->imageDataOrigin, dispFrame->height*dispFrame->width * sizeof(unsigned char));


		for (size_t i =0; i<blobs->updatedBlobSize; i++)
		{
			Blob& blob = *(blobs->updatedBlob[i]);
			char str[11];
			char area[50];
			int number = blob.id;
			sprintf(str, "%d", number);
			sprintf(area, "R: %f", blob.keyPoint.response);
			CvSize size = CvSize((int)blob.keyPoint.size, (int)blob.keyPoint.size);
			cvEllipse(dispFrame, cvPoint(blob.keyPoint.pt.x, blob.keyPoint.pt.y), size, 0, 0, 360, cvScalar(255), CV_FILLED, CV_AA);
			cvPutText(dispFrame, str, cvPoint(blob.keyPoint.pt.x - 15, blob.keyPoint.pt.y - 15), &font, cvScalar(128));
			cvPutText(dispFrame, area, cvPoint(blob.keyPoint.pt.x, blob.keyPoint.pt.y), &font, cvScalar(128));
		}
	}
	if (dispFrame != NULL)
	{
		std::ostringstream oss;
		oss << "~ " << this->timer->getAverageFPS() << " FPS";
		cvRectangle(dispFrame, cvPoint(0, 0), cvPoint(150, 20), cvScalar(0), CV_FILLED, CV_AA);
		cvRectangle(dispFrame, cvPoint(0, 0), cvPoint(150, 20), cvScalar(255), 2, CV_AA);
		cvPutText(dispFrame, oss.str().c_str(), cvPoint(5, 15), &font, cvScalar(255));
		cvShowImage((this->property("id")).asString().c_str(), dispFrame);
		cvWaitKey(1);
	}

	if(packet->isLocalCopy())
		delete packet;
	this->input->unlock();
#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("nuiDebugVideoSink::update frame rendering");
#endif

}

void nuiDebugVideoSink::start()
{
	
	nuiModule::start();
	
	LOG(NUI_DEBUG, "starting video sink");
}

void nuiDebugVideoSink::stop()
{
	
	nuiModule::stop();
}

void nuiDebugVideoSink::onSetupThread()
{
	//TODO: fix strange behaviour when restarting workflow  
	//(when breakpoint is set everything works fine, when not window doesn't appear)
	if (!wndInit)
	{
		cvNamedWindow((this->property("id")).asString().c_str(), CV_WINDOW_AUTOSIZE);
		wndInit = true;
		cvWaitKey(50);
	}
}

void nuiDebugVideoSink::onExitThread()
{
	if (wndInit)
	{
		cvDestroyWindow((this->property("id")).asString().c_str());
		wndInit = false;
		cvWaitKey(50);
	}
}

void nuiDebugVideoSink::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}