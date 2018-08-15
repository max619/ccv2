/**
* \file      ocvRealsenseWrapper.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "ocvRealsenseWrapper.h"

ocvRealsenseWrapper::ocvRealsenseWrapper()
{
	mtx.lock();
	depth_sensor = NULL;
	_img = NULL;
	procres = NULL;
	opened = false;
	ctx.set_devices_changed_callback([&](rs2::event_information& info)
	{
		container.updateDevices(info);
	});

	nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();

	processor = new oclRSWorldProcessor();	

	screenpoints = new CvPoint2D32f[4];

	

	perspectiveTransformMatrix = cvCreateMat(3, 3, CV_32F);

	dstscreenpoints = new CvPoint2D32f[4];

	isPlaneInit = false;
	rs2::context ctx;
	container.initDevices(ctx);

	if (factory.isOpenClSupported())
	{
		factory.initProgram(processor);
	}
	mtx.unlock();
}

ocvRealsenseWrapper::~ocvRealsenseWrapper()
{
	close();
	delete processor;
	cvReleaseMat(&perspectiveTransformMatrix);
	delete screenpoints;
	delete dstscreenpoints;

	cvReleaseImage(&_img);
}

bool ocvRealsenseWrapper::isOpen()
{
	return opened;
}

bool ocvRealsenseWrapper::open(int index)
{
	mtx.lock();
	try
	{
		pipe = container.getPipeline(index, frameSize.width, frameSize.height, fps);
		pipeIndex = index;
						
		rs2_error* error;
		rs2::depth_sensor _depth_sensor = container.getPipelineProfile(container.getDeviceAt(0)).get_device().first<rs2::depth_sensor>();
		
		intrisnic = pipe.get_active_profile().get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();
		if (container.getPipelineProfile(container.getDeviceAt(0)).get_device().is<rs400::advanced_mode>())
		{
			adv = &(container.getPipelineProfile(container.getDeviceAt(0)).get_device().as<rs400::advanced_mode>());
			// Check if advanced-mode is enabled
			if (!adv->is_enabled())
			{
				// Enable advanced-mode
				adv->toggle_advanced_mode(true);
			}
			advancedModeEnabled = true;
		}
		else
		{
			advancedModeEnabled = false;
		}
		depth_scale = _depth_sensor.get_depth_scale();
		depth_sensor = _depth_sensor;
		opened = true;
	}
	catch (_exception& ex)
	{

	}
	mtx.unlock();

	return opened;
}

bool ocvRealsenseWrapper::close()
{
	mtx.lock();
	if (opened)
	{
		pipe.stop();
		opened = false;
		isPlaneInit = false;
	}
	mtx.unlock();

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

IplImage* ocvRealsenseWrapper::queryWorldCoordinates()
{
	mtx.lock();
#ifdef ALLOW_BENCHMARKING	
	benchmark.startBencmarking();
#endif
	if (procres == NULL)
	{
		//procres = cvCreateImage(cvSize(intrisnic.width, intrisnic.height), IPL_DEPTH_32F, 4);
		if (intrisnic.width < 1 || intrisnic.height < 1)
			return NULL;
		procres = cvCreateImage(cvSize(intrisnic.width, intrisnic.height), IPL_DEPTH_8U, 1);
	}
	frameset = pipe.wait_for_frames();
	auto depthFrame = frameset.get_depth_frame().as<rs2::depth_frame>();
	void* data_ptr = (void*)depthFrame.get_data();

	if (!isPlaneInit)
	{
		p0 = getWorldCoordAt(depthFrame, pos[0].x, pos[0].y, intrisnic, depth_scale);
		Eigen::Vector3f p1 = getWorldCoordAt(depthFrame, pos[1].x, pos[1].y, intrisnic, depth_scale);
		Eigen::Vector3f p2 = getWorldCoordAt(depthFrame, pos[2].x, pos[2].y, intrisnic, depth_scale);
		Eigen::Vector3f v0 = p1 - p0;
		Eigen::Vector3f v1 = p2 - p0;
		n = v1.cross(v0);
		isPlaneInit = true;
	}	

#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("ocvRealsenseWrapper::queryWorldCoordinates depth frame query");
#endif

	nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();

	//TODO: Optimize all processing (put in one kernel)
	if (factory.isOpenClSupported())
	{

#ifdef ALLOW_BENCHMARKING	
		benchmark.startBencmarking();
#endif
		processor->getTouchedPoints((uint16_t*)data_ptr, depth_scale, intrisnic, n, p0, thresh, /*perspectiveTransformMatrix,*/ procres);

#ifdef ALLOW_BENCHMARKING	
		benchmark.stopBenchmarking("ocvRealsenseWrapper::queryWorldCoordinates processor->processWorld");
#endif
	}
	else
	{

#ifdef ALLOW_BENCHMARKING	
		benchmark.startBencmarking();
#endif

		processor->getTouchedPointsCpu((uint16_t*)data_ptr, depth_scale, intrisnic, n, p0, thresh, procres);

#ifdef ALLOW_BENCHMARKING	
		benchmark.stopBenchmarking("ocvRealsenseWrapper::queryWorldCoordinates processor->getTouchedPointsCpu");
#endif
	}
	/*if(shouldWarp)
		cvWarpPerspective(procres, procres, perspectiveTransformMatrix);*/


	mtx.unlock();
	return procres;
}

void ocvRealsenseWrapper::setOptionValue(rs2_option option, float val)
{
	mtx.lock();
	//pipe.stop();
	if(depth_sensor.supports(option) && !depth_sensor.is_option_read_only(option))
		depth_sensor.set_option(option, val);
	//pipe.start();
	mtx.unlock();
}

void ocvRealsenseWrapper::setOptionsRange(std::map<rs2_option, float>& options)
{
	mtx.lock();
	//pipe.stop();
	for (std::map<rs2_option, float>::iterator it = options.begin(); it != options.end(); it++)
	{
	
		if (depth_sensor.supports(it->first) && !depth_sensor.is_option_read_only(it->first))
			depth_sensor.set_option(it->first, it->second);
	}
	//pipe = container.getPipeline(pipeIndex, frameSize.width, frameSize.height, fps);
	mtx.unlock();
}

float ocvRealsenseWrapper::getOptionValue(rs2_option option)
{
	if (depth_sensor.supports(option))
		return depth_sensor.get_option(option);
	else
		return -10000.f;
}
