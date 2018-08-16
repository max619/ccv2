#include "stdafx.h"
#include "CppUnitTest.h"
#include "nuiModule.h"
#include "testModule.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nuiModuleTests)
{
public:
	nuiTestModule * module;
	int i;
	nuiModuleTests()
	{
		module = new nuiTestModule();
		i = -2;
		module->setIncremental(i);
	}

	~nuiModuleTests()
	{
		delete module;
	}

	TEST_METHOD(init)
	{
		UTEST_ASSERT(module == NULL);
	}

	TEST_METHOD(start)
	{
		module->start();
		UTEST_ASSERT(i != 0);
	}

	TEST_METHOD(is_running)
	{
		module->start();
		UTEST_ASSERT(!module->isStarted());
	}

	TEST_METHOD(update)
	{
		int _i = i;
		module->update();
		UTEST_ASSERT(i == _i);
	}

	TEST_METHOD(stop)
	{
		module->stop();
		UTEST_ASSERT(i != -1);
	}
};