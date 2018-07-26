#include "nuiRealSenseModule.h"

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(RealSenseModule)

MODULE_DECLARE(RealSenseModule, "native", "Input module to grab video from webcameras.");


nuiRealSenseModule::nuiRealSenseModule() : nuiModule() {
	MODULE_INIT();
	img = NULL;
	this->output = new nuiEndpoint(this);
	this->output->setTypeDescriptor(std::string("IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->output);

	this->outputDataPacket = new nuiRealSenseModuleDataPacket();
}

nuiRealSenseModule::~nuiRealSenseModule() {
	realsenseW.close();
	cvReleaseImage(&img);
}

void nuiRealSenseModule::update() {
	this->output->lock();
	this->output->clear();

#ifdef BENCHMARK_NUI_REALSENSE_MODULE_H	
		QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif

	IplImage* timg = realsenseW.queryWorldCoordinates();

#ifdef BENCHMARK_NUI_REALSENSE_MODULE_H	
	QueryPerformanceCounter(&performanceCountNDRangeStop);
	QueryPerformanceFrequency(&perfFrequency);
	LogInfo("TOTAL: queryWorldCoordinates took %f ms.\n",
		1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);

#endif
	if (timg != NULL)
	{
		this->outputDataPacket->packData(timg);
		this->output->setData(this->outputDataPacket);
		this->output->transmitData();
	}

	this->output->unlock();
	cvReleaseImage(&timg);
}

void nuiRealSenseModule::start() {
	LOG(NUI_DEBUG, "Starting nuiRealSenseModule");
	int camid = this->hasProperty("camid") ? this->property("camid").asInteger() : 0;
	if (realsenseW.open(camid))
		nuiModule::start();
}
