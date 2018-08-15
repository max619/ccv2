/**
* \file      nuiOpenClProgram.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef OPENCL_PROGRAM
#define OPENCL_PROGRAM

#include "nuiOpenClStructures.h"

#define DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()\
public:\
	char * getSourceCode(int id);\
	int getFunctionsCount();\



class nuiOpenClAlgorithm
{
public:
	nuiOpenClAlgorithm(int functions);
	~nuiOpenClAlgorithm();

	virtual char* getSourceCode(int id);
	virtual int getFunctionsCount();
	const char* getFunctionName(int id);
	ocl_container* getOclFunctionPtr(int id);

protected:
	ocl_container** ocl;
	const char** functionsNames;
	cl_uint getAllignedBufferSize(cl_uint width, cl_uint height, cl_uint elementsize);
	void* allocAlligned(cl_uint alignedsize);
	int ExecKernel(ocl_container * container, cl_uint width, cl_uint height);
};
#endif