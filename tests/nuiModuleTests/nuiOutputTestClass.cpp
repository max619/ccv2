#include "stdafx.h"
#include "nuiOutputTestClass.h"

MODULE_DECLARE(OutputTestClass, "native", "Filter out background");

nuiOutputTestClass::nuiOutputTestClass() : nuiModule()
{
	MODULE_INIT();

	outputEndpoint = new nuiEndpoint(this);
	outputEndpoint->setTypeDescriptor("int");
	datapacket = new nuiIntDataPacket();

	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, outputEndpoint);

	this->setInputEndpointCount(0);

}


nuiOutputTestClass::~nuiOutputTestClass()
{
}

void nuiOutputTestClass::update()
{
	outputEndpoint->lock();
	outputEndpoint->clear();
	i++;
	datapacket->packData(&i);
	outputEndpoint->setData(datapacket);
	outputEndpoint->transmitData();
	outputEndpoint->unlock();
}

void nuiOutputTestClass::start()
{
	i = 0;
	nuiModule::start();
}

void nuiOutputTestClass::stop()
{
	nuiModule::stop();
}


void nuiOutputTestClass::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}
