#pragma once
#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <librealsense2\rs.hpp>
#include "rs2DeviceContainer.h"
#include <Windows.h>

class ocvRealsenseWrapper
{
public:
	ocvRealsenseWrapper();
	~ocvRealsenseWrapper();

	bool open(int index);
	bool close();

	bool queryColorFrame(IplImage ** img);
	bool queryDepthFrame(IplImage ** img);

	IplImage * thresholdDepthImage(float min, float max);

private:
	/*rs2::video_frame colorFrame;
	rs2::frame depthFrame;*/
	rs2::colorizer color_map;
	rs2::pipeline pipe;
	rs2::frameset frameset;
	rs2::context ctx;
	rs2DeviceContainer container;

	cv::Mat depthImage;
	cv::Mat colorImage;
	cv::Mat colorImageBgr;
	IplImage* _img;

	bool opened;
};
