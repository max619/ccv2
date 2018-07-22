#pragma once
#include <opencv2\core.hpp>
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
	rs2::frame depthFrame, colorFrame;
	rs2::colorizer color_map;
	rs2::pipeline pipe;
	rs2::frameset frameset;

	IplImage* depthImage;
	IplImage* colorImage;

	bool opened;
};

