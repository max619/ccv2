#include "stdafx.h"
#include "nuiInputTestClass.h"

MODULE_DECLARE(InputTestClass, "native", "Filter out background");

nuiInputTestClass::nuiInputTestClass() : nuiModule()
{
	MODULE_INIT();

	inputEndpoint = new nuiEndpoint(this);
	inputEndpoint->setTypeDescriptor("int");

	this->setOutputEndpointCount(0);
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, inputEndpoint);
}


nuiInputTestClass::~nuiInputTestClass()
{
}

void nuiInputTestClass::update()
{
	this->inputEndpoint->lock();
	nuiIntDataPacket* packet =  (nuiIntDataPacket*)this->inputEndpoint->getData();
	void* _i;
	packet->unpackData(_i);

	*i = *(int*)_i;
	
	this->inputEndpoint->unlock();
}

void nuiInputTestClass::setIncremental(int & i)
{
	this->i = &i;
}


void nuiInputTestClass::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}
