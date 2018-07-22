#include "ocvRealsenseWrapper.h"



ocvRealsenseWrapper::ocvRealsenseWrapper()
{
	depthImage = NULL;
	colorImage = NULL;
}


ocvRealsenseWrapper::~ocvRealsenseWrapper()
{
	close();
}

bool ocvRealsenseWrapper::open(int index)
{
	try
	{
		pipe.start();
		opened = true;
	}
	catch (_exception ex)
	{

	}

	return opened;
}

bool ocvRealsenseWrapper::close()
{
	if (opened)
	{
		pipe.stop();
		opened = false;
	}

	return !opened;
}

bool ocvRealsenseWrapper::queryColorFrame(IplImage ** img)
{

	try
	{
		frameset = pipe.wait_for_frames();
		auto colorFrame = frameset.get_color_frame().as<rs2::video_frame>();
		void* data_ptr = (void*)colorFrame.get_data();
		IplImage * image = *img;

		colorImage.release();

		colorImage = cv::Mat(cv::Size(colorFrame.get_width(), colorFrame.get_height()), CV_8UC3, data_ptr, cv::Mat::AUTO_STEP);

		cv::cvtColor(colorImage, colorImageBgr, cv::COLOR_BGR2RGB);

		image = new IplImage(colorImageBgr);
		img[0] = image;
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

bool ocvRealsenseWrapper::queryDepthFrame(IplImage ** img)
{
	return false;
}
