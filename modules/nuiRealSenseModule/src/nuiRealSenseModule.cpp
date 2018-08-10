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

	LinkProperty("x0", NUI_PROPERTY_INTEGER, realsenseW.pos[0].x);
	LinkProperty("y0", NUI_PROPERTY_INTEGER, realsenseW.pos[0].y);
	LinkProperty("x1", NUI_PROPERTY_INTEGER, realsenseW.pos[1].x);
	LinkProperty("y1", NUI_PROPERTY_INTEGER, realsenseW.pos[1].y);
	LinkProperty("x2", NUI_PROPERTY_INTEGER, realsenseW.pos[2].x);
	LinkProperty("y2", NUI_PROPERTY_INTEGER, realsenseW.pos[2].y);

	LinkProperty("min", NUI_PROPERTY_DOUBLE, min);
	LinkProperty("max", NUI_PROPERTY_DOUBLE, max);

	LinkProperty("fps", NUI_PROPERTY_INTEGER, realsenseW.fps);

	LinkProperty("frameWidth", NUI_PROPERTY_INTEGER, realsenseW.frameSize.width);
	LinkProperty("frameHeight", NUI_PROPERTY_INTEGER, realsenseW.frameSize.height);

}

nuiRealSenseModule::~nuiRealSenseModule() {
	realsenseW.close();
	cvReleaseImage(&img);
}

void nuiRealSenseModule::update() {
	this->output->lock();
	this->output->clear();

#ifdef ALLOW_BENCHMARKING	
	benchmark.startBencmarking();
#endif

	IplImage* timg = realsenseW.queryWorldCoordinates();

#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("nuiRealSenseModule::update realsenseW.queryWorldCoordinates()");
#endif
	if (timg != NULL)
	{
		this->outputDataPacket->packData(timg);
		this->output->setData(this->outputDataPacket);
		this->output->transmitData();
	}

	this->output->unlock();
	//cvReleaseImage(&timg);
}

void nuiRealSenseModule::start() {
	LOG(NUI_DEBUG, "Starting nuiRealSenseModule");
	initAnchorPoints();
	int camid = this->hasProperty("camid") ? this->property("camid").asInteger() : 0;
	if (realsenseW.open(camid))
		nuiModule::start();
}

void nuiRealSenseModule::initAnchorPoints()
{
	//this->readLinkedProperties();
	realsenseW.thresh.x() = min / 1000.f;
	realsenseW.thresh.y() = max / 1000.f;
	nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();
	factory.platformName = this->hasProperty("platformName") ? this->property("platformName").asString() : "Intel";
}

void nuiRealSenseModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{
	if (name == "min")
	{
		realsenseW.thresh.x() = prop->asDouble() / 1000.f;
	} else
	if (name == "max")
	{
		realsenseW.thresh.y() = prop->asDouble() / 1000.f;
	}
}
