/**
* \file      nuiOpenClFactory.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef OPENCL_FACTORY
#define OPENCL_FACTORY

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include "CL\cl.h"
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>
#include <string>

#include "nuiOpenClProgram.h"
#include "nuiOpenClStructures.h"


class nuiOpenClFactory
{
public:
	static nuiOpenClFactory& getInstance();
	std::string platformName;

	nuiOpenClFactory();
	~nuiOpenClFactory();

	bool isOpenClSupported();
	int initProgram(nuiOpenClAlgorithm* program);

private:
	cl_int err;
	cl_device_type deviceType = CL_DEVICE_TYPE_GPU;

	bool CheckPreferredPlatformMatch(cl_platform_id platform, const char * preferredPlatform);
	cl_platform_id FindOpenCLPlatform(const char * preferredPlatform, cl_device_type deviceType);
	int GetPlatformAndDeviceVersion(cl_platform_id platformId, ocl_container * ocl);
	int SetupOpenCL(ocl_container * ocl, cl_device_type deviceType);
	int CreateAndBuildProgram(ocl_container * ocl, const char* program);
	int CreateAndBuildProgramFromFile(ocl_container * ocl, const char * path);


};
#endif