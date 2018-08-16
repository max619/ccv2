#include "stdafx.h"
#include "CppUnitTest.h"
#include "nuiModule.h"
#include "nuiInputTestClass.h"
#include "nuiOutputTestClass.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nuiDataStreamTests)
{
public:
	nuiInputTestClass * inputClass;
	nuiOutputTestClass * outputClass;
	nuiDataStream* dataStream;
	int i;

	nuiDataStreamTests()
	{
		i = 0;
		inputClass = new nuiInputTestClass();
		outputClass = new nuiOutputTestClass();
		nuiEndpoint* srcEndpoint = outputClass->getOutputEndpoint(0);
		nuiEndpoint* dstEndpoint = inputClass->getInputEndpoint(0);
		dataStream = srcEndpoint->addConnection(dstEndpoint);

		dataStream->setDeepCopy(true);
		dataStream->setAsyncMode(true);

		outputClass->property("oscillator_mode").set(true);
		outputClass->property("oscillator_wait").set(15);

		inputClass->setIncremental(i);
	}

	~nuiDataStreamTests()
	{
		delete inputClass;
		delete outputClass;
	}

	TEST_METHOD(run)
	{
		i = 0;
		inputClass->start();
		outputClass->start();

		Sleep(300);

		outputClass->stop();
		inputClass->stop();

		UTEST_ASSERT(i == 0);
	}
};