/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ocvRealsenseWrapper.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef OCV_REALSENSE_WRAPPER
#define OCV_REALSENSE_WRAPPER

#define EIGEN_DONT_ALIGN_STATICALLY

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
#include "helpers.h"
#include "oclRSWorldProcessor.h"

#ifdef ALLOW_BENCHMARKING	
#include "nuiBenchmark.h"
#endif

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
	CvPoint2D32f* screenpoints;
	CvSize2D32f destsize; 

	Eigen::Vector2f thresh; 
	Eigen::Vector3f n, p0;
	CvPoint pos[3];
	CvSize frameSize;
	int fps;
	bool shouldWarp;

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
	oclRSWorldProcessor* processor;
	IplImage* procres;
	CvPoint2D32f* dstscreenpoints;
	CvMat* perspectiveTransformMatrix;
	bool isPlaneInit;
	

	rs2_intrinsics intrisnic;
	float depth_scale;
	Eigen::Quaternionf q;

	bool opened;

#ifdef ALLOW_BENCHMARKING	
	nuiBenchmark benchmark;
#endif
};
#endif
