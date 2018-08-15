/**
 * \file      nuiEndpoint.cpp
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \author    Maxim Bagryantsev
 * \date      2012-2018
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 * \copyright Copyright 2018 Appollo Pro. All rights reserved.
 */

#include "nuiEndpoint.h"
#include "nuiModule.h"

nuiEndpoint::nuiEndpoint(nuiModule *hostModule)
{
	mtx = new pt::mutex();
	moduleHoster = hostModule;
	dataPacket = NULL;
	descriptor = NULL;
}

nuiEndpoint::~nuiEndpoint()
{
	removeConnections();
	delete mtx;
}

void nuiEndpoint::writeData(nuiDataPacket* dataPacket)
{
	mtx->lock();
	if (this->dataPacket != NULL && this->dataPacket->isLocalCopy())
	{
		delete this->dataPacket;
		this->dataPacket = NULL;
	}
	this->dataPacket = dataPacket;
	if (moduleHoster != NULL)
		moduleHoster->notifyDataReceived(this);
	mtx->unlock();
}

nuiEndpoint* nuiEndpoint::getConnectedEndpointAtIndex(int index)
{
	int i = 0;
	for (std::map<nuiEndpoint*, nuiDataStream*>::iterator iter = dataStreams.begin();
		iter != dataStreams.end(); iter++, i++)
	{
		if (i == index)
			return iter->first;
	}
	return NULL;
}


void nuiEndpoint::transmitData()
{
	mtx->lock();
	for (std::map<nuiEndpoint*, nuiDataStream*>::iterator iter = dataStreams.begin();
		iter != dataStreams.end(); iter++)
	{
		if (!iter->second->isRunning())
			iter->second->startStream();

		iter->second->sendData(dataPacket);
	}
	mtx->unlock();
}

nuiEndpointDescriptor* nuiEndpoint::getEndpointDescriptor()
{
	if (descriptor == NULL)
	{
		descriptor = new nuiEndpointDescriptor(getTypeDescriptor());
		descriptor->setDescriptor(this->getTypeDescriptor());
	}
	return descriptor;
}

nuiDataStream* nuiEndpoint::addConnection(nuiEndpoint *endpoint)
{
	if ((endpoint == NULL) || (!canBePairedWithEndpoint(endpoint)))
		return NULL;

	std::map<nuiEndpoint*, nuiDataStream*>::iterator iter = dataStreams.find(endpoint);
	if (iter != dataStreams.end())
		return NULL;

	dataStreams[endpoint] = new nuiDataStream();
	dataStreams[endpoint]->setReceiver(*endpoint);

	return dataStreams[endpoint];
}

nuiDatastreamError::err nuiEndpoint::removeConnection(nuiEndpoint *endpoint)
{
	std::map<nuiEndpoint*, nuiDataStream*>::iterator iter = dataStreams.find(endpoint);
	if (iter == dataStreams.end())
		return nuiDatastreamError::NonexistentEndpoint;
	iter->second->stopStream();
	delete iter->second;
	dataStreams.erase(iter);
	return nuiDatastreamError::Success;
}

void nuiEndpoint::removeConnections()
{
	while (!dataStreams.empty())
	{
		std::map<nuiEndpoint*, nuiDataStream*>::iterator iter = dataStreams.begin();
		iter->second->stopStream();
		delete iter->second;
		dataStreams.erase(iter);
	}
}

nuiDataStream* nuiEndpoint::getDataStreamForEndpoint(nuiEndpoint *endpoint)
{
	std::map<nuiEndpoint*, nuiDataStream*>::iterator iter = dataStreams.find(endpoint);
	if (iter == dataStreams.end())
		return NULL;
	return iter->second;
}

unsigned int nuiEndpoint::getConnectionCount()
{
	return dataStreams.size();
}

void nuiEndpoint::setTypeDescriptor(std::string typeDescriptor)
{
	this->typeDescriptor = typeDescriptor;
}

void nuiEndpoint::setData(nuiDataPacket *dataPacket)
{
	mtx->lock();
	if (canBeSettedData(dataPacket))
	{
		if (this->dataPacket != NULL)
		{
			delete dataPacket;
		}
		this->dataPacket = dataPacket;
	}
	mtx->unlock();
}

nuiDataPacket *nuiEndpoint::getData()
{
	mtx->lock();
	nuiDataPacket* data = dataPacket;
	dataPacket = NULL;
	mtx->unlock();
	return data;
}

std::string nuiEndpoint::getTypeDescriptor()
{
	return typeDescriptor;
}

bool nuiEndpoint::canBePairedWithEndpoint(nuiEndpoint *endpoint)
{
	if (endpoint == NULL)
		return false;
	if ((endpoint->typeDescriptor == "*") || (this->typeDescriptor == "*"))
		return true;
	return nuiUtils::inList(endpoint->getTypeDescriptor(), this->getTypeDescriptor(), ",");
}

bool nuiEndpoint::canBeSettedData(nuiDataPacket *dataPacket)
{
	//! \todo (dataPacket->getDataPacketType() == NULL) should rise error?
	if ((dataPacket == NULL) || (dataPacket->getDataPacketType() == NULL) ||
		(dataPacket->getDataPacketType() == "*"))
		return true;

	return nuiUtils::inList(std::string(dataPacket->getDataPacketType()),
		this->getTypeDescriptor(), ",");
}

void nuiEndpoint::lock()
{
	mtx->lock();
}

void nuiEndpoint::unlock()
{
	mtx->unlock();
}

void nuiEndpoint::clear()
{
	nuiDataPacket* packet = getData();
	if (packet != NULL && packet->isLocalCopy())
		delete packet;
	setData(NULL);
}

void nuiEndpoint::setModuleHoster(nuiModule* moduleHoster)
{
	this->moduleHoster = moduleHoster;
}

nuiModule *nuiEndpoint::getModuleHoster()
{
	return moduleHoster;
}

nuiEndpointDescriptor::nuiEndpointDescriptor(std::string typeDescriptor)
{
	this->typeDescriptor = typeDescriptor;
}

std::string &nuiEndpointDescriptor::getDescriptor()
{
	return typeDescriptor;
}

void nuiEndpointDescriptor::setDescriptor(std::string typeDescriptor)
{
	this->typeDescriptor = typeDescriptor;
}

void nuiEndpointDescriptor::setParentModuleDescriptor(nuiModuleDescriptor* parentModuleDescriptor)
{
	this->parentModuleDescriptor = parentModuleDescriptor;
}

nuiModuleDescriptor* nuiEndpointDescriptor::getParentModuleDescriptor()
{
	return this->parentModuleDescriptor;
}

int nuiEndpointDescriptor::getIndex()
{
	return index;
}

void nuiEndpointDescriptor::setIndex(int index)
{
	this->index = index;
}
