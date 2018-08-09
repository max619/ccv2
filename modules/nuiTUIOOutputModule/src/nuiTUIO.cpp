//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiTUIOOutputModule
// Purpose:		Output TUIO
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

//#include "ofxNCore.h"
#include "nuiTUIO.h"
#include "..\..\..\libs\nuiSystem\inc\nuiDebugLogger.h"


MODULE_DECLARE(TUIOOutputModule, "native", "Output TUIO")

nuiTUIOOutputModule::nuiTUIOOutputModule() : nuiModule()
{
	MODULE_INIT();

	this->_pInput = new nuiEndpoint(this);
	this->_pInput->setTypeDescriptor(std::string(NUI_DATAPACKET_THROUGH_BLOBVECTOR_PACKET_DATA_TYPE));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0,this->_pInput);
};

nuiTUIOOutputModule::~nuiTUIOOutputModule()
{
	delete server;
};

void nuiTUIOOutputModule::update()
{
	BlobVector* data;
	void* dataPacket;

	nuiDataPacket* packet = this->_pInput->getData();
	if(packet == NULL) 
		return;
	packet->unpackData(dataPacket);
	data = (BlobVector*)dataPacket;
	if (data == NULL)
		return;

	ProcessBlobVector(data);

	if (packet->isLocalCopy())
		delete packet;
};

void nuiTUIOOutputModule::start()
{
	if (this->hasProperty("host") && this->hasProperty("port"))
	{/*
		host = (char*)();
		port = ;*/
		server = new TUIO2::TuioServer(this->property("host").asString().c_str(), this->property("port").asInteger());
	}
	else
		server = new TUIO2::TuioServer();
	
	if (this->hasProperty("flashhost") && this->hasProperty("flashmeth"))
	{
		flashhost = this->property("flashhost").asString().c_str();
		flashmeth = this->property("flashmeth").asString().c_str();
		server->addOscSender(new TUIO2::FlashSender(flashhost, flashmeth));
	}

	char name[25];
	sprintf(name, "CCV 2.0 TUIO ID=%d", this->property("id").asInteger());

	server->setSourceName("t2sim");

	nuiModule::start();
	LOG(NUI_DEBUG, "Started nuiTUIOOutputModule");
	LOG(NUI_DEBUG, name);
};

void nuiTUIOOutputModule::stop()
{
	nuiModule::stop();
}
void nuiTUIOOutputModule::ProcessBlobVector(BlobVector * vector)
{
	frameTime = TUIO2::TuioTime::getSystemTime();
	server->initTuioFrame(frameTime);
	trackEvents(vector);
	server->stopUntouchedMovingObjects();
	server->commitTuioFrame();

	// simulate 50Hz compensating the previous processing time
	int d = 20 - (TUIO2::TuioTime::getSystemTime().getTotalMilliseconds() - frameTime.getTotalMilliseconds());
	Sleep(d);
}
void nuiTUIOOutputModule::trackEvents(BlobVector * vector)
{
	int width = vector->targetResolution.width;
	int height = vector->targetResolution.height;

	for (size_t i = 0; i < vector->newBlobsSize; i++)
	{
		Blob b = *(vector->newBlobs[i]);
		TUIO2::TuioObject* obj = server->createTuioPointer(b.keyPoint.pt.x / width, b.keyPoint.pt.y / height, 0, 0, b.keyPoint.size / (2 * width), 0);
		TUIO2::TuioPointer* pointer = obj->getTuioPointer();
		pointer->getTuioTime() = frameTime;
		trackedPointers.emplace(BLOB_POINTER_PAIR(b.id, pointer));
	}

	for (size_t i = 0; i < vector->updatedBlobSize; i++)
	{
		Blob b = *(vector->updatedBlob[i]); 

		BLOB_POINTER_MAP::iterator pair  = (trackedPointers.find(b.id));
		if (pair != trackedPointers.end())
		{
			TUIO2::TuioPointer* pointer = (*pair).second; 
			pointer->getTuioTime() = frameTime;
			server->updateTuioPointer(pointer, b.keyPoint.pt.x / width, b.keyPoint.pt.y / height, 0, 0, b.keyPoint.size / (2 * width), 0);
		}
	}

	for (size_t i = 0; i < vector->removedBlobsSize; i++)
	{
		Blob b = *(vector->removedBlobs[i]);
		BLOB_POINTER_MAP::iterator pair = (trackedPointers.find(b.id));
		if (pair != trackedPointers.end())
		{
			TUIO2::TuioPointer* pointer = (*pair).second;
			pointer->getTuioTime() = frameTime;
			trackedPointers.erase(b.id);
			server->removeTuioPointer(pointer);
		}
	}

	for (BLOB_POINTER_MAP::iterator it = trackedPointers.begin(); it != trackedPointers.end(); it++)
	{
		bool found = false;
		long id = (*it).first;
		for (size_t i = 0; i < vector->size && !found; i++)
		{
			if (vector->blobs[i].id == id)
				found = true;
		}
		if (!found)
		{
			TUIO2::TuioPointer* pointer = (*it).second;
			server->removeTuioPointer(pointer);
			trackedPointers.erase(id);
		}
	}
}

void nuiTUIOOutputModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{

}