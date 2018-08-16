#include "stdafx.h"
#include "CppUnitTest.h"
#include "nuiModule.h"
#include "testModule.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nuiModuleOscillatorTests)
{
public:
	nuiTestModule * module;
	int i;
	nuiModuleOscillatorTests()
	{
		module = new nuiTestModule();
		i = -2;
		module->setIncremental(i);
		module->property("oscillator_mode").set(true);
		module->property("use_thread").set(true);
	}

	~nuiModuleOscillatorTests()
	{
		delete module;
	}

	TEST_METHOD(start)
	{
		module->start();
		Sleep(150);
		module->stop();
		UTEST_ASSERT(i == 0);
	}

};