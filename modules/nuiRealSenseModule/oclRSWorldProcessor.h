/////////////////////////////////////////////////////////////////////////////
// Name:        modules/oclRSWorldProcessor.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef OCL_RS_WORLD_PROCESSOR
#define OCL_RS_WORLD_PROCESSOR
//#define BENCHMARK_OCL_RS_WORLD_PROCESSOR

#include "static.h"
#include "nuiFrameworkManager.h"
#include "nuiOpenCl.h"
#include "CL/cl.hpp"
#include <mutex>
#include <opencv2/core.hpp>
#include <Eigen/Geometry>
#include <librealsense2/rs.hpp>
#include <Windows.h>
#include "helpers.h"


class oclRSWorldProcessor : public nuiOpenClAlgorithm
{
public:
	oclRSWorldProcessor();
	~oclRSWorldProcessor();
	void processWorld(uint16_t* data, float& scale, rs2_intrinsics& intrisnic, Eigen::Quaternionf& quaterninon, IplImage* res);

	void processWorldCpu(uint16_t* data, float& scale, rs2_intrinsics& intrisnic, Eigen::Quaternionf& quaterninon, IplImage* res);
	void getTouchedPointsCpu(uint16_t* data, float& scale, rs2_intrinsics& intrisnic, Eigen::Vector3f& normal, Eigen::Vector3f& basept, Eigen::Vector2f& threshold, IplImage* res);

#ifdef BENCHMARK_OCL_RS_WORLD_PROCESSOR
	LARGE_INTEGER perfFrequency;
	LARGE_INTEGER performanceCountNDRangeStart;
	LARGE_INTEGER performanceCountNDRangeStop;
#endif
private:
	std::mutex mutex;
	bool clmeminit;
	cl_mem inputMem;
	cl_mem outputMem;
	cl_int inputSize, outputSize;
	void* input;
	void* output;
	int initMem(ocl_container* container, int width, int height);

	DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()
};

#endif // !OCL_RS_WORLD_PROCESSOR



