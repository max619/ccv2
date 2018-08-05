/////////////////////////////////////////////////////////////////////////////
// Name:        modules/oclThreshold.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 NUI Group
/////////////////////////////////////////////////////////////////////////////

#ifndef OCL_THRESHOLD
#define OCL_THRESHOLD
#include "nuiOpenClProgram.h"
#include "opencv2/core.hpp"
#include <CL/cl.hpp>
#include <mutex>

#ifdef ALLOW_BENCHMARKING	
#include "nuiBenchmark.h"
#endif

class oclThreshold : public nuiOpenClAlgorithm
{
public:
	oclThreshold();
	~oclThreshold();

	IplImage* calcThreshold(uint16_t * src, uint width, uint height, uint16_t min, uint16_t max);

	DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()


private:
#ifdef ALLOW_BENCHMARKING
	nuiBenchmark bencmark;
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

