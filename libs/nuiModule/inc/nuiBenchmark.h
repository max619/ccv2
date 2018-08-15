/**
* \file      nuiBenchmark.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef NUI_BENCHMARK
#define NUI_BENCHMARK

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>

class nuiBenchmark
{
public:
	nuiBenchmark();
	~nuiBenchmark();

	void startBencmarking();
	void stopBenchmarking(std::string module_name);

private:
#ifdef _WIN32
	LARGE_INTEGER perfFrequency;
	LARGE_INTEGER performanceCountNDRangeStart;
	LARGE_INTEGER performanceCountNDRangeStop;
#endif
};

#endif