#pragma once
#include "nuiOpenClProgram.h"
#include "opencv2/core.hpp"
#include <CL/cl.hpp>

class oclThreshold : public nuiOpenClProgram
{
public:
	oclThreshold();
	~oclThreshold();

	DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()

	IplImage* calcThreshold(IplImage* src, float min, float max);

};

