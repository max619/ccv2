/////////////////////////////////////////////////////////////////////////////
// Name:        modules/ocl3DPointCloudRotation.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef OCL_3D_POINT_CLOUD_ROTATION
#define OCL_3D_POINT_CLOUD_ROTATION

#include "nuiOpenCl.h"
#include "CL/cl.hpp"
#include "Eigen/Geometry"
#include <mutex>
#include "nuiFrameworkManager.h"
#include "static.h"
#include <opencv2/core.hpp>
#include "helpers.h"

class ocl3DPointCloudRotation : public nuiOpenClAlgorithm
{
public:
	ocl3DPointCloudRotation();
	~ocl3DPointCloudRotation();

	IplImage* rotate(IplImage* src, Eigen::Quaternion<float> quaterninon);

	DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()

private:
#ifdef _DEBUG
	LARGE_INTEGER perfFrequency;
	LARGE_INTEGER performanceCountNDRangeStart;
	LARGE_INTEGER performanceCountNDRangeStop;
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
