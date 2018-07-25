/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ocvRealsenseWrapper.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef OCV_REALSENSE_WRAPPER
#define OCV_REALSENSE_WRAPPER

#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <librealsense2\rs.hpp>
#include "rs2DeviceContainer.h"
#include <Windows.h>
#include "oclThreshold.h"
#include "oclDepthToWorld.h"
#include "ocl3DPointCloudRotation.h"
#include "nuiOpenCl.h"
#include "Eigen/Geometry"

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

	IplImage * queryWorldCoordinates();

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
	oclThreshold* threshold;
	oclDepthToWorld* depthToWorld;
	ocl3DPointCloudRotation* rotation;

	bool opened;
};
#endif
