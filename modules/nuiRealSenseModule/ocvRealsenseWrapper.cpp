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
	return false;
}