#pragma once
#include "nuiOpenClProgram.h"
#include "opencv2/core.hpp"
#include <CL/cl.hpp>
#include <mutex>

#ifdef _DEBUG
#include <Windows.h>
#endif

class oclThreshold : public nuiOpenClProgram
{
public:
	oclThreshold();
	~oclThreshold();

	DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()

	IplImage* calcThreshold(uint16_t * src, uint width, uint height, uint16_t min, uint16_t max);
	bool clmeminit;
	
	std::mutex mutex;
#ifdef _DEBUG
	LARGE_INTEGER perfFrequency;
	LARGE_INTEGER performanceCountNDRangeStart;
	LARGE_INTEGER performanceCountNDRangeStop;
#endif

private:
	cl_mem inputMem;
	cl_mem outputMem;
	cl_int inputSize, outputSize;
	void* input;
	void* output;
	int initMem(ocl_container* container, int width, int height);
};

