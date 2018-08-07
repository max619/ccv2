#include "ocvRealsenseWrapper.h"



ocvRealsenseWrapper::ocvRealsenseWrapper()
{
	_img = NULL;
	procres = NULL;
	ctx.set_devices_changed_callback([&](rs2::event_information& info)
	{
		container.updateDevices(info);
	});

	nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();

	threshold = new oclThreshold();
	depthToWorld = new oclDepthToWorld();
	rotation = new ocl3DPointCloudRotation();
	processor = new oclRSWorldProcessor();	

	screenpoints = new CvPoint2D32f[4];

	

	perspectiveTransformMatrix = cvCreateMat(3, 3, CV_32F);

	dstscreenpoints = new CvPoint2D32f[4];

	isPlaneInit = false;
}


ocvRealsenseWrapper::~ocvRealsenseWrapper()
{
	close();
	if (threshold != NULL)
	{
		delete threshold;
		threshold = NULL;
	}
	cvReleaseMat(&perspectiveTransformMatrix);
	delete screenpoints;
	delete dstscreenpoints;

	cvReleaseImage(&_img);
}

bool ocvRealsenseWrapper::open(int index)
{
	try
	{
		rs2::context ctx;
		container.initDevices(ctx);
		pipe = container.getPipeline(index, frameSize.width, frameSize.height, fps);
		opened = true;

		nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();

		if (factory.isOpenClSupported())
		{
			/*factory.initProgram(threshold);
			factory.initProgram(depthToWorld);
			factory.initProgram(rotation);*/
			factory.initProgram(processor);
		}

		Eigen::Vector3f a(1., 1., 0.);
		Eigen::Vector3f b(1., 0.5, 3.);
		rs2_error* error;
		rs2::depth_sensor depth_sensor = container.getPipelineProfile(container.getDeviceAt(0)).get_device().first<rs2::depth_sensor>();
		
		intrisnic = pipe.get_active_profile().get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();
		depth_scale = depth_sensor.get_depth_scale();

		/*dstscreenpoints[0] = CvPoint2D32f(0, 0);
		dstscreenpoints[1] = CvPoint2D32f(destsize.width, 0);
		dstscreenpoints[2] = CvPoint2D32f(destsize.width, destsize.height);
		dstscreenpoints[3] = CvPoint2D32f(0, destsize.height);
		cvGetPerspectiveTransform(screenpoints, dstscreenpoints, perspectiveTransformMatrix);
		

		q = Eigen::Quaternionf::FromTwoVectors(a, b);*/
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

	nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();
	rs2::depth_sensor depth_sensor = container.getPipelineProfile(container.getDeviceAt(0)).get_device().first<rs2::depth_sensor>();
	float depth_scale = depth_sensor.get_depth_scale();
	if (factory.isOpenClSupported())
	{
		return threshold->calcThreshold((uint16_t*)data_ptr, depthFrame.get_width(), depthFrame.get_height(), min / depth_scale, max / depth_scale);
	}
	else
	{
		IplImage* res = cvCreateImage(CvSize(depthFrame.get_width(), depthFrame.get_height()), IPL_DEPTH_8U, 1);

		uint16_t umin = min / depth_scale;
		uint16_t umax = max / depth_scale;
#ifdef _OPENMP
#pragma omp parallel for
#endif		
		for (int y = 0; y < depthFrame.get_height(); y++)
		{
			unsigned char* res_ptr = reinterpret_cast<unsigned char*>(res->imageData + (y * depthFrame.get_width() * sizeof(unsigned char)));
			uint16_t* src_ptr = reinterpret_cast<uint16_t*>((uint16_t*)data_ptr + (y * depthFrame.get_width() * sizeof(unsigned char)));
			for (int x = 0; x < depthFrame.get_width(); x++, res_ptr++, src_ptr++)
			{
				uint16_t f_px = *src_ptr;
				if (f_px > umin && f_px < umax)
				{
					(*res_ptr) = 255;
				}
				else
				{
					(*res_ptr) = 0;
				}
			}
		}


		return res;
	}

}

IplImage* ocvRealsenseWrapper::queryWorldCoordinates()
{
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
	if(shouldWarp)
		cvWarpPerspective(procres, procres, perspectiveTransformMatrix);

	
	return procres;
}