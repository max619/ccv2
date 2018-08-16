#include "stdafx.h"
#include "CppUnitTest.h"
#include "nuiProperty.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(nuiLinkedPropertyTests)
{
public:

	TEST_METHOD(link_float)
	{
		float v = 0.f;
		nuiProperty p(NUI_PROPERTY_FLOAT);
		nuiLinkedProperty* linkedProp = p.linkProperty("v", NUI_PROPERTY_FLOAT, &v);
		p.set(1.f);

		if (v != 1.f)
			Assert::Fail(L"Linked prop not updated");

		delete linkedProp;
	}


	TEST_METHOD(link_double)
	{
		double v = 0.;
		nuiProperty p(NUI_PROPERTY_DOUBLE);
		nuiLinkedProperty* linkedProp = p.linkProperty("v", NUI_PROPERTY_DOUBLE, &v);
		p.set(1.);

		if (v != 1.)
			Assert::Fail(L"Linked prop not updated");

		delete linkedProp;
	}


	TEST_METHOD(link_int)
	{
		int v = 0;
		nuiProperty p(NUI_PROPERTY_INTEGER);
		nuiLinkedProperty* linkedProp = p.linkProperty("v", NUI_PROPERTY_INTEGER, &v);
		p.set(1);

		if (v != 1)
			Assert::Fail(L"Linked prop not updated");

		delete linkedProp;
	}


	TEST_METHOD(link_bool)
	{
		bool v = false;
		nuiProperty p(NUI_PROPERTY_BOOL);
		nuiLinkedProperty* linkedProp = p.linkProperty("v", NUI_PROPERTY_BOOL, &v);
		p.set(true);

		if (v != true)
			Assert::Fail(L"Linked prop not updated");

		delete linkedProp;
	}
};