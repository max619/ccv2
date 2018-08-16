#include "stdafx.h"
#include "CppUnitTest.h"
#include "nuiThread.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void incInt(nuiThread* thread)
{
	
	int& i = *(int*)thread->getUserData();;
	i++;	
}


TEST_CLASS(nuiThreadTests)
{
public:

	TEST_METHOD(execute)
	{
		int i = 0;
		nuiThread* t = new nuiThread(incInt, &i);
		t->execute();

		if (i != 1)
			Assert::Fail(L"Not incremented");

		delete t;
	}

	TEST_METHOD(wait)
	{
		int i = 0;
		nuiThread* t = new nuiThread(incInt, &i);
		t->start();

		t->relax(10);

		t->stop();

		if (i == 0)
			Assert::Fail(L"Not incremented");

		delete t;
	}
};