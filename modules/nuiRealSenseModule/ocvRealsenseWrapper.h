#pragma once
#include <opencv2\core.hpp>
#include <opencv2/imgproc.hpp>
#include <librealsense2\rs.hpp>

class ocvRealsenseWrapper
{
public:
	ocvRealsenseWrapper();
	~ocvRealsenseWrapper();

	bool open(int index);
	bool close();

	bool queryColorFrame(IplImage ** img);
	bool queryDepthFrame(IplImage ** img);

private:
	/*rs2::video_frame colorFrame;
	rs2::frame depthFrame;*/
	rs2::colorizer color_map;
	rs2::pipeline pipe;
	rs2::frameset frameset;

	cv::Mat depthImage;
	cv::Mat colorImage;
	cv::Mat colorImageBgr;

	bool opened;
};

