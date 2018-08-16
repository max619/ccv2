#include "stdafx.h"
#include "CppUnitTest.h"
#include "nuiProperty.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace nuiSystemTests
{
	TEST_CLASS(nuiPropertyTests)
	{
	public:

		TEST_METHOD(constructor_float)
		{
			nuiProperty* p = new nuiProperty(1.f, "float");
			if (p->asFloat() != 1.f)
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "float")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_double)
		{
			nuiProperty* p = new nuiProperty(1., "double");
			if (p->asDouble() != 1.)
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "double")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_bool)
		{
			nuiProperty* p = new nuiProperty(true, "bool");
			if (p->asBool() != true)
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "bool")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_string)
		{
			nuiProperty* p = new nuiProperty(std::string("str"), "string");
			if (p->asString() != "str")
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "string")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_int)
		{
			nuiProperty* p = new nuiProperty(1, "int");
			if (p->asInteger() != 1)
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "int")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_type_int)
		{
			nuiProperty* p = new nuiProperty(NUI_PROPERTY_INTEGER);
			p->setDescription("int");
			p->set(1);
			if (p->asInteger() != 1)
				Assert::Fail(L"Value equality check failed");
			p->set(1.);
			if (p->asInteger() != 1)
				Assert::Fail(L"Value equality check failed");
			p->set(1.f);
			if (p->asInteger() != 1)
				Assert::Fail(L"Value equality check failed");
			p->set(true);
			if (p->asInteger() != 1)
				Assert::Fail(L"Value equality check failed");
			p->set("1");
			if (p->asInteger() != 1)
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "int")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_type_double)
		{
			nuiProperty* p = new nuiProperty(NUI_PROPERTY_DOUBLE);
			p->setDescription("double");
			p->set(1);
			if (p->asDouble() != 1.)
				Assert::Fail(L"Value equality check failed");
			p->set(1.);
			if (p->asDouble() != 1.)	
				Assert::Fail(L"Value equality check failed");
			p->set(1.f);
			if (p->asDouble() != 1.)	
				Assert::Fail(L"Value equality check failed");
			p->set(true);
			if (p->asDouble() != 1.)	
				Assert::Fail(L"Value equality check failed");
			p->set("1");
			if (p->asDouble() != 1.)	
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "double")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_type_float)
		{
			nuiProperty* p = new nuiProperty(NUI_PROPERTY_FLOAT);
			p->setDescription("float");
			p->set(1);
			if (p->asFloat() != 1.f)
				Assert::Fail(L"Value equality check failed");
			p->set(1.);
			if (p->asFloat() != 1.f)
				Assert::Fail(L"Value equality check failed");
			p->set(1.f);
			if (p->asFloat() != 1.f)
				Assert::Fail(L"Value equality check failed");
			p->set(true);
			if (p->asFloat() != 1.f)
				Assert::Fail(L"Value equality check failed");
			p->set("1");
			if (p->asFloat() != 1.f)
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "float")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_type_string)
		{
			nuiProperty* p = new nuiProperty(NUI_PROPERTY_STRING);
			p->setDescription("string");
			p->set(1);
			if (p->asString() != "1")
				Assert::Fail(L"Value equality check failed");
			p->set(1.);
			if (p->asString() != "1.000000")
				Assert::Fail(L"Value equality check failed");
			p->set(1.f);
			if (p->asString() != "1.000000")
				Assert::Fail(L"Value equality check failed");
			p->set(true);
			if (p->asString() != "1")
				Assert::Fail(L"Value equality check failed");
			p->set("1");
			if (p->asString() != "1")
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "string")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(constructor_type_bool)
		{
			nuiProperty* p = new nuiProperty(NUI_PROPERTY_BOOL);
			p->setDescription("double");
			p->set(1);
			if (p->asBool() != true)
				Assert::Fail(L"Value equality check failed");
			p->set(1.);
			if (p->asBool() != true)
				Assert::Fail(L"Value equality check failed");
			p->set(1.f);
			if (p->asBool() != true)
				Assert::Fail(L"Value equality check failed");
			p->set(true);
			if (p->asBool() != true)
				Assert::Fail(L"Value equality check failed");
			p->set("true");
			if (p->asBool() != true)
				Assert::Fail(L"Value equality check failed");
			if (p->getDescription() != "double")
				Assert::Fail(L"Description equality check failed");

			delete p;
		}

		TEST_METHOD(is_text_string)
		{
			nuiProperty* p = new nuiProperty("1");
			
			if(!p->isText())
				Assert::Fail(L"Not text");

			delete p;
		}

		TEST_METHOD(is_text_bool)
		{
			nuiProperty* p = new nuiProperty(true);

			if (p->isText())
				Assert::Fail(L"Is text");

			delete p;
		}


		TEST_METHOD(is_text_int)
		{
			nuiProperty* p = new nuiProperty(1);

			if (p->isText())
				Assert::Fail(L"Is text");

			delete p;
		}


		TEST_METHOD(is_text_double)
		{
			nuiProperty* p = new nuiProperty(1.);

			if (p->isText())
				Assert::Fail(L"Is text");

			delete p;
		}

		TEST_METHOD(is_text_float)
		{
			nuiProperty* p = new nuiProperty(1.f);

			if (p->isText())
				Assert::Fail(L"Is text");

			delete p;
		}

		TEST_METHOD(get_type_float)
		{
			if (nuiProperty::getPropertyTypeName(NUI_PROPERTY_FLOAT) != "float")
				Assert::Fail(L"Inavlid type");
		}

		TEST_METHOD(get_type_bool)
		{
			if (nuiProperty::getPropertyTypeName(NUI_PROPERTY_BOOL) != "bool")
				Assert::Fail(L"Inavlid type");
		}
		
		TEST_METHOD(get_type_double)
		{
			if (nuiProperty::getPropertyTypeName(NUI_PROPERTY_DOUBLE) != "double")
				Assert::Fail(L"Inavlid type");
		}
		
		TEST_METHOD(get_type_string)
		{
			if (nuiProperty::getPropertyTypeName(NUI_PROPERTY_STRING) != "string")
				Assert::Fail(L"Inavlid type");
		}
		
		TEST_METHOD(get_type_int)
		{
			if (nuiProperty::getPropertyTypeName(NUI_PROPERTY_INTEGER) != "integer")
				Assert::Fail(L"Inavlid type");
		}	
	};
}