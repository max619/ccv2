/////////////////////////////////////////////////////////////////////////////
// Name:        modules/oclDepthToWorld.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 Appollo Pro
/////////////////////////////////////////////////////////////////////////////

#ifndef OCL_DEPTH_TO_WORLD
#define OCL_DEPTH_TO_WORLD

#include <CL/cl.hpp>
#include <nuiOpenCl.h>
#include <librealsense2/rs.hpp>
#include <opencv2/core.hpp>
#include <mutex>
#include <nuiFrameworkManager.h>
//#include "nuiRealSenseModule.h"
#include "static.h"
#ifdef ALLOW_BENCHMARKING
#include "nuiBenchmark.h"
#endif

class oclDepthToWorld : public nuiOpenClAlgorithm
{
public:
	oclDepthToWorld();
	~oclDepthToWorld();

	IplImage* calcWorldCoordinatesNormal(uint16_t* data, float scale, uint width, uint height, rs2_intrinsics intrisnic);
	IplImage * calcWorldCoordinatesInverseBrownConrady(uint16_t * data, float scale, uint width, uint height, rs2_intrinsics intrisnic);


	DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()
private:
#ifdef ALLOW_BENCHMARKING
	nuiBenchmark benchmark;
#endif
	std::mutex mutex;
	bool clmeminit;
	cl_mem inputMem;
	cl_mem outputMem;
	cl_int inputSize, outputSize;
	void* input;
	void* output;
	int initMem(ocl_container* container, int width, int height);
};

#endif

