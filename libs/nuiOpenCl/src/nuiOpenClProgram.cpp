/**
* \file      nuiOpenClProgram.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "..\include\nuiOpenClProgram.h"

nuiOpenClAlgorithm::nuiOpenClAlgorithm(int functions)
{
	ocl = new ocl_container*[functions];

	for (int i = 0; i < functions; i++)
		ocl[i] = new ocl_container();

}

nuiOpenClAlgorithm::~nuiOpenClAlgorithm()
{
	int funcs = getFunctionsCount();
	
	delete [] ocl;
	delete [] functionsNames;
}

char * nuiOpenClAlgorithm::getSourceCode(int id)
{
	return NULL;
}

int nuiOpenClAlgorithm::getFunctionsCount()
{
	return 0;
}

const char * nuiOpenClAlgorithm::getFunctionName(int id)
{
	return functionsNames[id];
}

ocl_container * nuiOpenClAlgorithm::getOclFunctionPtr(int id)
{
	return ocl[id];
}

cl_uint nuiOpenClAlgorithm::getAllignedBufferSize(cl_uint width, cl_uint height, cl_uint elementsize)
{
	return ((elementsize * width * height - 1) / 64 + 1) * 64;
}

void * nuiOpenClAlgorithm::allocAlligned(cl_uint alignedsize)
{
	return _aligned_malloc(alignedsize, 4096);
}

int nuiOpenClAlgorithm::ExecKernel(ocl_container* container, cl_uint width, cl_uint height)
{
	cl_int err = CL_SUCCESS;
	size_t globalWorkSize[2] = { width, height };

	err = clEnqueueNDRangeKernel(container->commandQueue, container->kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueNDRangeKernel returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	err = clFinish(container->commandQueue); if (CL_SUCCESS != err)
	{
		LogError("Error: clFinish return %s\n", TranslateOpenCLError(err));
		return err;
	}
}
