#include "ocvRealsenseWrapper.h"



ocvRealsenseWrapper::ocvRealsenseWrapper()
{
	_img = NULL;

	ctx.set_devices_changed_callback([&](rs2::event_information& info)
	{
		container.updateDevices(info);
	});
}


ocvRealsenseWrapper::~ocvRealsenseWrapper()
{
	close();
}

bool ocvRealsenseWrapper::open(int index)
{
	try
	{
		rs2::context ctx;
		container.initDevices(ctx);
		pipe = container.getPipeline(index);
		opened = true;
	}
	catch (_exception& ex)
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
		colorImage.release();
		colorImageBgr.release();

		colorImage = cv::Mat(cv::Size(colorFrame.get_width(), colorFrame.get_height()), CV_8UC3, data_ptr, cv::Mat::AUTO_STEP);

		cv::cvtColor(colorImage, colorImageBgr, cv::COLOR_BGR2RGB);

		_img = new IplImage(colorImageBgr);
		img[0] = _img;
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

bool ocvRealsenseWrapper::queryDepthFrame(IplImage ** img)
{
	try
	{
		frameset = pipe.wait_for_frames();
		auto depthFrame = frameset.get_depth_frame().as<rs2::depth_frame>();
		void* data_ptr = (void*)depthFrame.get_data();
		depthImage.release();

		depthImage = cv::Mat(cv::Size(depthFrame.get_width(), depthFrame.get_height()), CV_FP16, data_ptr, cv::Mat::AUTO_STEP);

		/*for (int y = 0; y < depthFrame.get_height(); y++)
			for (int x = 0; x < depthFrame.get_width(); x++)
			{
				depthImage.at<float>(y, x) = depthFrame.get_distance(x, y);
			}*/

		_img = new IplImage(depthImage);
		img[0] = _img;
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

IplImage* ocvRealsenseWrapper::thresholdDepthImage(float min, float max)
{
	frameset = pipe.wait_for_frames();
	auto depthFrame = frameset.get_depth_frame().as<rs2::depth_frame>();
	void* data_ptr = (void*)depthFrame.get_data();


	IplImage* res = cvCreateImage(CvSize(depthFrame.get_width(), depthFrame.get_height()), IPL_DEPTH_8U, 1);



#ifdef _OPENMP
#pragma omp parallel for

	for (int y = 0; y < depthFrame.get_height(); y++)
	{
		unsigned char* res_ptr = reinterpret_cast<unsigned char*>(res->imageData + (y * depthFrame.get_width() * sizeof(unsigned char)));
		for (int x = 0; x < depthFrame.get_width(); x++, res_ptr++)
		{
			float f_px = depthFrame.get_distance(x, y);
			if (f_px > min && f_px < max)
			{
				(*res_ptr) = 255;
			}
			else
			{
				(*res_ptr) = 0;
			}
		}
	}

#else
	unsigned char* res_ptr = reinterpret_cast<unsigned char*>(res->imageData);

	for (int y = 0; y < depthFrame.get_height(); y++)
	{
		for (int x = 0; x < depthFrame.get_width(); x++, res_ptr++)
		{
			float f_px = depthFrame.get_distance(x, y);
			if (f_px > min && f_px < max)
			{
				(*res_ptr) = 255;
			}
			else
			{
				(*res_ptr) = 0;
			}
		}
}
#endif


	return res;
}