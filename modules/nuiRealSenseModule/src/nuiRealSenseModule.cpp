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

	LinkCameraProps();

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
	{
		for (std::map<rs2_option, float>::iterator it = cameraProps.begin(); it != cameraProps.end(); it++)
		{
			if (it->second == -10000.f)
			{
				float val = realsenseW.getOptionValue(it->first);
				properties.find(rs2_option_to_string(it->first))->second->set(val, false);
				it->second = val;
			}
		}

		realsenseW.setOptionsRange(cameraProps);
	}
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
	}
	else if (name == "max")
	{
		realsenseW.thresh.y() = prop->asDouble() / 1000.f;
	}
	else if (name.find("RS2") >= 0 && realsenseW.isOpen())
	{
		rs2_option option = rs2_option::RS2_OPTION_COUNT;
		for (std::map<rs2_option, float>::iterator it = cameraProps.begin(); it != cameraProps.end() && option == rs2_option::RS2_OPTION_COUNT; it++)
		{
			if (&(it->second) == linkedProp->prop)
			{
				option = it->first;
			}
		}

		if (option != rs2_option::RS2_OPTION_COUNT && realsenseW.isOpen())
		{
			realsenseW.setOptionValue(option, prop->asFloat());
		}
	}
}

void nuiRealSenseModule::LinkCameraProps() {
	cameraProps[RS2_OPTION_EXPOSURE] = -10000.f;
	LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_EXPOSURE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_EXPOSURE], -10000.f); /**< Controls exposure time of color camera. Setting any value will disable auto exposure*/
	cameraProps[RS2_OPTION_GAIN] = -10000.f;
	LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_GAIN), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_GAIN], -10000.f); /**< Color image gain*/
	cameraProps[RS2_OPTION_ENABLE_AUTO_EXPOSURE] = -10000.f;
	LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_ENABLE_AUTO_EXPOSURE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_ENABLE_AUTO_EXPOSURE], -10000.f); /**< Enable / disable color image auto-exposure*/
	cameraProps[RS2_OPTION_LASER_POWER] = -10000.f;
	LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_LASER_POWER), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_LASER_POWER], -10000.f); /**< Power of the F200 / SR300 projector with 0 meaning projector off*/
	cameraProps[RS2_OPTION_EMITTER_ENABLED] = -10000.f;
	LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_EMITTER_ENABLED), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_EMITTER_ENABLED], -10000.f); /**< Laser Emitter enabled */
	cameraProps[RS2_OPTION_FRAMES_QUEUE_SIZE] = -10000.f;
	LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_FRAMES_QUEUE_SIZE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_FRAMES_QUEUE_SIZE], -10000.f); /**< Number of frames the user is allowed to keep per stream. Trying to hold-on to more frames will cause frame-drops.*/
	cameraProps[RS2_OPTION_DEPTH_UNITS] = -10000.f;
	LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_DEPTH_UNITS), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_DEPTH_UNITS], -10000.f); /**< Number of meters represented by a single depth unit */

	//cameraProps[RS2_OPTION_BACKLIGHT_COMPENSATION] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_BACKLIGHT_COMPENSATION), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_BACKLIGHT_COMPENSATION], -10000.f); /**< Enable / disable color backlight compensation*/
	//cameraProps[RS2_OPTION_BRIGHTNESS] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_BRIGHTNESS), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_BRIGHTNESS], -10000.f); /**< Color image brightness*/
	//cameraProps[RS2_OPTION_CONTRAST] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_CONTRAST), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_CONTRAST], -10000.f); /**< Color image contrast*/
	//cameraProps[RS2_OPTION_GAMMA] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_GAMMA), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_GAMMA], -10000.f); /**< Color image gamma setting*/
	//cameraProps[RS2_OPTION_HUE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_HUE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_HUE], -10000.f); /**< Color image hue*/
	//cameraProps[RS2_OPTION_SATURATION] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_SATURATION), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_SATURATION], -10000.f); /**< Color image saturation setting*/
	//cameraProps[RS2_OPTION_SHARPNESS] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_SHARPNESS), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_SHARPNESS], -10000.f); /**< Color image sharpness setting*/
	//cameraProps[RS2_OPTION_WHITE_BALANCE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_WHITE_BALANCE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_WHITE_BALANCE], -10000.f); /**< Controls white balance of color image. Setting any value will disable auto white balance*/
	//cameraProps[RS2_OPTION_ENABLE_AUTO_WHITE_BALANCE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_ENABLE_AUTO_WHITE_BALANCE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_ENABLE_AUTO_WHITE_BALANCE], -10000.f); /**< Enable / disable color image auto-white-balance*/
	//cameraProps[RS2_OPTION_VISUAL_PRESET] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_VISUAL_PRESET), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_VISUAL_PRESET], -10000.f); /**< Provide access to several recommend sets of option presets for the depth camera */
	//cameraProps[RS2_OPTION_ACCURACY] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_ACCURACY), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_ACCURACY], -10000.f); /**< Set the number of patterns projected per frame. The higher the accuracy value the more patterns projected. Increasing the number of patterns help to achieve better accuracy. Note that this control is affecting the Depth FPS */
	//cameraProps[RS2_OPTION_MOTION_RANGE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_MOTION_RANGE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_MOTION_RANGE], -10000.f); /**< Motion vs. Range trade-off with lower values allowing for better motion sensitivity and higher values allowing for better depth range*/
	//cameraProps[RS2_OPTION_FILTER_OPTION] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_FILTER_OPTION), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_FILTER_OPTION], -10000.f); /**< Set the filter to apply to each depth frame. Each one of the filter is optimized per the application requirements*/
	//cameraProps[RS2_OPTION_CONFIDENCE_THRESHOLD] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_CONFIDENCE_THRESHOLD), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_CONFIDENCE_THRESHOLD], -10000.f); /**< The confidence level threshold used by the Depth algorithm pipe to set whether a pixel will get a valid range or will be marked with invalid range*/	
	//cameraProps[RS2_OPTION_TOTAL_FRAME_DROPS] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_TOTAL_FRAME_DROPS), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_TOTAL_FRAME_DROPS], -10000.f); /**< Total number of detected frame drops from all streams */
	//cameraProps[RS2_OPTION_AUTO_EXPOSURE_MODE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_AUTO_EXPOSURE_MODE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_AUTO_EXPOSURE_MODE], -10000.f); /**< Auto-Exposure modes: Static Anti-Flicker and Hybrid */
	//cameraProps[RS2_OPTION_POWER_LINE_FREQUENCY] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_POWER_LINE_FREQUENCY), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_POWER_LINE_FREQUENCY], -10000.f); /**< Power Line Frequency control for anti-flickering Off/50Hz/60Hz/Auto */
	//cameraProps[RS2_OPTION_ASIC_TEMPERATURE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_ASIC_TEMPERATURE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_ASIC_TEMPERATURE], -10000.f); /**< Current Asic Temperature */
	//cameraProps[RS2_OPTION_ERROR_POLLING_ENABLED] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_ERROR_POLLING_ENABLED), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_ERROR_POLLING_ENABLED], -10000.f); /**< disable error handling */
	//cameraProps[RS2_OPTION_PROJECTOR_TEMPERATURE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_PROJECTOR_TEMPERATURE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_PROJECTOR_TEMPERATURE], -10000.f); /**< Current Projector Temperature */
	//cameraProps[RS2_OPTION_OUTPUT_TRIGGER_ENABLED] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_OUTPUT_TRIGGER_ENABLED), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_OUTPUT_TRIGGER_ENABLED], -10000.f); /**< Enable / disable trigger to be outputed from the camera to any external device on every depth frame */
	//cameraProps[RS2_OPTION_MOTION_MODULE_TEMPERATURE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_MOTION_MODULE_TEMPERATURE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_MOTION_MODULE_TEMPERATURE], -10000.f); /**< Current Motion-Module Temperature */
	//cameraProps[RS2_OPTION_ENABLE_MOTION_CORRECTION] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_ENABLE_MOTION_CORRECTION), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_ENABLE_MOTION_CORRECTION], -10000.f); /**< Enable/Disable automatic correction of the motion data */
	//cameraProps[RS2_OPTION_AUTO_EXPOSURE_PRIORITY] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_AUTO_EXPOSURE_PRIORITY), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_AUTO_EXPOSURE_PRIORITY], -10000.f); /**< Allows sensor to dynamically ajust the frame rate depending on lighting conditions */
	//cameraProps[RS2_OPTION_COLOR_SCHEME] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_COLOR_SCHEME), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_COLOR_SCHEME], -10000.f); /**< Color scheme for data visualization */
	//cameraProps[RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED], -10000.f); /**< Perform histogram equalization post-processing on the depth data */
	//cameraProps[RS2_OPTION_MIN_DISTANCE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_MIN_DISTANCE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_MIN_DISTANCE], -10000.f); /**< Minimal distance to the target */
	//cameraProps[RS2_OPTION_MAX_DISTANCE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_MAX_DISTANCE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_MAX_DISTANCE], -10000.f); /**< Maximum distance to the target */
	//cameraProps[RS2_OPTION_TEXTURE_SOURCE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_TEXTURE_SOURCE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_TEXTURE_SOURCE], -10000.f); /**< Texture mapping stream unique ID */
	//cameraProps[RS2_OPTION_FILTER_MAGNITUDE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_FILTER_MAGNITUDE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_FILTER_MAGNITUDE], -10000.f); /**< The 2D-filter effect. The specific interpretation is given within the context of the filter */
	//cameraProps[RS2_OPTION_FILTER_SMOOTH_ALPHA] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_FILTER_SMOOTH_ALPHA), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_FILTER_SMOOTH_ALPHA], -10000.f); /**< 2D-filter parameter controls the weight/radius for smoothing.*/
	//cameraProps[RS2_OPTION_FILTER_SMOOTH_DELTA] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_FILTER_SMOOTH_DELTA), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_FILTER_SMOOTH_DELTA], -10000.f); /**< 2D-filter range/validity threshold*/
	//cameraProps[RS2_OPTION_HOLES_FILL] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_HOLES_FILL), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_HOLES_FILL], -10000.f); /**< Enhance depth data post-processing with holes filling where appropriate*/
	//cameraProps[RS2_OPTION_STEREO_BASELINE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_STEREO_BASELINE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_STEREO_BASELINE], -10000.f); /**< The distance in mm between the first and the second imagers in stereo-based depth cameras*/
	//cameraProps[RS2_OPTION_AUTO_EXPOSURE_CONVERGE_STEP] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_AUTO_EXPOSURE_CONVERGE_STEP), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_AUTO_EXPOSURE_CONVERGE_STEP], -10000.f); /**< Allows dynamically ajust the converge step value of the target exposure in Auto-Exposure algorithm*/
	//cameraProps[RS2_OPTION_INTER_CAM_SYNC_MODE] = -10000.f;
	//LinkPropertyAndSetDefaultVal(rs2_option_to_string(RS2_OPTION_INTER_CAM_SYNC_MODE), NUI_PROPERTY_FLOAT, cameraProps[RS2_OPTION_INTER_CAM_SYNC_MODE], -10000.f); /**< Impose Inter-camera HW synchronization mode. Applicable for D400/Rolling Shutter SKUs */
}
