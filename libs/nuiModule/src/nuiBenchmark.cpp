/**
* \file      nuiBenchmark.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiBenchmark.h"

nuiBenchmark::nuiBenchmark()
{
}

nuiBenchmark::~nuiBenchmark()
{
}

void nuiBenchmark::startBencmarking()
{
	//TODO: Add Linux impl
#ifdef WIN32
	QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif
}

void nuiBenchmark::stopBenchmarking(std::string module_name)
{
	//TODO: Add Linux impl
#ifdef WIN32
	QueryPerformanceCounter(&performanceCountNDRangeStop);
	QueryPerformanceFrequency(&perfFrequency);
#endif
	std::string message = "Processing of " + module_name + "took %f ms.\n";
	printf(message.c_str(),
		1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
}
