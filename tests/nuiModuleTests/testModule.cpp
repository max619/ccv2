#include "stdafx.h"
#include "testModule.h"

MODULE_DECLARE(TestModule, "native", "Filter out background");


nuiTestModule::nuiTestModule() : nuiModule()
{
	MODULE_INIT();
}


nuiTestModule::~nuiTestModule()
{
}

void nuiTestModule::update()
{
	(*i)++;
}

void nuiTestModule::start()
{
	*i = 0;
	nuiModule::start();
}

void nuiTestModule::stop()
{
	*i = -1;
	nuiModule::stop();
}

void nuiTestModule::setIncremental(int & i)
{
	this->i = &i;
}

void nuiTestModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}
