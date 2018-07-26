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
	threshold = NULL;
	if (factory.isOpenClSupported())
	{
		threshold = new oclThreshold();
		depthToWorld = new oclDepthToWorld();
		rotation = new ocl3DPointCloudRotation();
		processor = new oclRSWorldProcessor();
	}
}


ocvRealsenseWrapper::~ocvRealsenseWrapper()
{
	close();
	if (threshold != NULL)
	{
		delete threshold;
		threshold = NULL;
	}

	cvReleaseImage(&_img);
}

bool ocvRealsenseWrapper::open(int index)
{
	try
	{
		rs2::context ctx;
		container.initDevices(ctx);
		pipe = container.getPipeline(index);
		opened = true;

		nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();

		if (factory.isOpenClSupported())
		{
			factory.initProgram(threshold);
			factory.initProgram(depthToWorld);
			factory.initProgram(rotation);
			factory.initProgram(processor);
		}

		Eigen::Vector3f a(1., 1., 0.);
		Eigen::Vector3f b(1., 0.5, 3.);
		rs2_error* error;
		rs2::depth_sensor depth_sensor = container.getPipelineProfile(container.getDeviceAt(0)).get_device().first<rs2::depth_sensor>();

		intrisnic = pipe.get_active_profile().get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();
		depth_scale = depth_sensor.get_depth_scale();

		q = Eigen::Quaternionf::FromTwoVectors(a, b);
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
#ifdef BENCHMARK_OCV_REALSENSE_WRAPPER	
	QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif
	if (procres == NULL)
	{
		procres = cvCreateImage(cvSize(intrisnic.width, intrisnic.height), IPL_DEPTH_32F, 4);
	}
	frameset = pipe.wait_for_frames();
	auto depthFrame = frameset.get_depth_frame().as<rs2::depth_frame>();
	void* data_ptr = (void*)depthFrame.get_data();

#ifdef BENCHMARK_OCV_REALSENSE_WRAPPER	
	QueryPerformanceCounter(&performanceCountNDRangeStop);
	QueryPerformanceFrequency(&perfFrequency);
	LogInfo("get_depth_frame took %f ms.\n",
		1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
#endif

	nuiOpenClFactory& factory = nuiOpenClFactory::getInstance();

	//TODO: Optimize all processing (put in one kernel)
	if (factory.isOpenClSupported())
	{

#ifdef BENCHMARK_OCV_REALSENSE_WRAPPER	
		QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif

#ifdef BENCHMARK_OCV_REALSENSE_WRAPPER	
		QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif

		processor->processWorld((uint16_t*)data_ptr, depth_scale, intrisnic, q, procres);

#ifdef BENCHMARK_OCV_REALSENSE_WRAPPER	
		QueryPerformanceCounter(&performanceCountNDRangeStop);
		QueryPerformanceFrequency(&perfFrequency);
		LogInfo("rotate took %f ms.\n",
			1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
#endif
	}
	else
	{

#ifdef BENCHMARK_OCV_REALSENSE_WRAPPER	
		QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif
		cl_float4 quat = castQuaternionToFloat4(q);
		

#ifdef _OPENMP
#pragma omp parallel for
#endif		
		for (int y = 0; y < depthFrame.get_height(); y++)
		{
			float* res_ptr = reinterpret_cast<float*>(procres->imageData + (y * depthFrame.get_width() * sizeof(float) * 4));
			uint16_t* src_ptr = reinterpret_cast<uint16_t*>((uint16_t*)data_ptr + (y * depthFrame.get_width() * sizeof(unsigned char)));
			cl_float4 fmul, res;
			float x1, y1, z1, w, u, v, ddepth;
			for (int x = 0; x < depthFrame.get_width(); x++, res_ptr++, src_ptr++)
			{
				u = (x - intrisnic.ppx) / intrisnic.fx;
				v = (y - intrisnic.ppy) / intrisnic.fy;
				ddepth = *src_ptr * depth_scale;

				x1 = u * ddepth;
				y1 = v * ddepth;
				z1 = ddepth;
				w = 0;
				
				fmul.w = w * quat.w - x1 * quat.x - y1 * quat.y - z1 * quat.z;
				fmul.x = w * quat.x + x1 * quat.w + y1 * quat.z + z1 * quat.y;
				fmul.y = w * quat.y + y1 * quat.w + z1 * quat.x + x1 * quat.z;
				fmul.z = w * quat.z + z1 * quat.w + x1 * quat.y + y1 * quat.x;

				res.w = fmul.w * quat.w + fmul.x * quat.x + fmul.y * quat.y + fmul.z * quat.z;
				res.x = -fmul.w * quat.x + fmul.x * quat.w - fmul.y * quat.z - fmul.z * quat.y;
				res.y = -fmul.w * quat.y + fmul.y * quat.w - fmul.z * quat.x - fmul.x * quat.z;
				res.z = -fmul.w * quat.z + fmul.z * quat.w - fmul.x * quat.y - fmul.y * quat.x;

				*res_ptr = res.w;
				res_ptr++;
				*res_ptr = res.x;
				res_ptr++;
				*res_ptr = res.y;
				res_ptr++;
				*res_ptr = res.z;
			}
		}

#ifdef BENCHMARK_OCV_REALSENSE_WRAPPER
		QueryPerformanceCounter(&performanceCountNDRangeStop);
		QueryPerformanceFrequency(&perfFrequency);
		LogInfo("process took %f ms.\n",
			1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
#endif
	}
	return procres;
}