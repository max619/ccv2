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
	realsenseW.pos[0] = CvPoint(this->hasProperty("x0") ? this->property("x0").asInteger() : 0, this->hasProperty("y0") ? this->property("y0").asInteger() : 0);
	realsenseW.pos[1] = CvPoint(this->hasProperty("x1") ? this->property("x1").asInteger() : 0, this->hasProperty("y1") ? this->property("y1").asInteger() : 0);
	realsenseW.pos[2] = CvPoint(this->hasProperty("x2") ? this->property("x2").asInteger() : 0, this->hasProperty("y2") ? this->property("y2").asInteger() : 0);

	realsenseW.thresh.x() = (float)(this->hasProperty("min") ? this->property("min").asDouble() : 0.) / 1000.f;
	realsenseW.thresh.y() = (float)(this->hasProperty("max") ? this->property("max").asDouble() : 0.) / 1000.f;

	realsenseW.destsize.width = this->hasProperty("screenWidth") ? this->property("screenWidth").asInteger() : 0;
	realsenseW.destsize.height = this->hasProperty("screenHeight") ? this->property("screenHeight").asInteger() : 0;

	realsenseW.screenpoints[0] = CvPoint2D32f(this->hasProperty("sx0") ? this->property("sx0").asDouble() : 0., this->hasProperty("sy0") ? this->property("sy0").asDouble() : 0.);
	realsenseW.screenpoints[1] = CvPoint2D32f(this->hasProperty("sx1") ? this->property("sx1").asDouble() : 0., this->hasProperty("sy1") ? this->property("sy1").asDouble() : 0.);
	realsenseW.screenpoints[2] = CvPoint2D32f(this->hasProperty("sx2") ? this->property("sx2").asDouble() : 0., this->hasProperty("sy2") ? this->property("sy2").asDouble() : 0.);
	realsenseW.screenpoints[3] = CvPoint2D32f(this->hasProperty("sx3") ? this->property("sx3").asDouble() : 0., this->hasProperty("sy3") ? this->property("sy3").asDouble() : 0.);
}
