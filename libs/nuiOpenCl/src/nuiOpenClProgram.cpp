#include "..\include\nuiOpenClProgram.h"

nuiOpenClProgram::nuiOpenClProgram(int functions)
{
	ocl = new ocl_container*[functions];

	for (int i = 0; i < functions; i++)
		ocl[i] = new ocl_container();

}

nuiOpenClProgram::~nuiOpenClProgram()
{
	int funcs = getFunctionsCount();

	for (int i = 0; i < funcs; i++)
	{
		delete ocl[i];
		delete functionsNames[i];
	}
	delete ocl;
	delete functionsNames;
}

char * nuiOpenClProgram::getSourceCode(int id)
{
	return NULL;
}

int nuiOpenClProgram::getFunctionsCount()
{
	return 0;
}

const char * nuiOpenClProgram::getFunctionName(int id)
{
	return functionsNames[id];
}

ocl_container * nuiOpenClProgram::getOclFunctionPtr(int id)
{
	return ocl[id];
}

cl_uint nuiOpenClProgram::getAllignedBufferSize(cl_uint width, cl_uint height, cl_uint elementsize)
{
	return ((elementsize * width * height - 1) / 64 + 1) * 64;
}

void * nuiOpenClProgram::allocAlligned(cl_uint alignedsize)
{
	return _aligned_malloc(alignedsize, 4096);
}

int nuiOpenClProgram::ExecKernel(ocl_container* container, cl_uint width, cl_uint height)
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
