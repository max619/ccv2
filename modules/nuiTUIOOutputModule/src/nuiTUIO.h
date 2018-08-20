//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiTUIOOutputModule
// Purpose:		Output TUIO
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////
/**
* \file      nuiTUIOOutputModule.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/


#ifndef NUI_TUIOOUTPUT_MODULE
#define NUI_TUIOOUTPUT_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include <nuiTrackerStructs.h>
#include <TuioServer.h>
#include <FlashSender.h>

#define BLOB_POINTER_PAIR std::pair<long, TUIO2::TuioPointer*>
#define BLOB_POINTER_MAP std::map<long, TUIO2::TuioPointer*>

class nuiTUIOOutputModule : public nuiModule
{
public:
	nuiTUIOOutputModule();
	~nuiTUIOOutputModule();

	void update();
	void start();
	void stop();

protected:
private:
	nuiEndpoint * _pInput;
	bool hostStarted;
	// TUIO broadcast params
	char*			host;
	int				port;
	const char*			flashhost;
	const char*			flashmeth;
	TUIO2::TuioTime		frameTime;

	// TUIO class for broadcasting
	TUIO2::TuioServer* server;
	//std::list<BlobTuioPointer*> trackedPointers;
	BLOB_POINTER_MAP trackedPointers;
	void ProcessBlobVector(BlobVector* vector);
	void trackEvents(BlobVector* vector);

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiTUIOOutputModule)
IMPLEMENT_DEALLOCATOR(nuiTUIOOutputModule)

START_IMPLEMENT_DESCRIPTOR(nuiTUIOOutputModule, "native", "Output TUIO")

descriptor->setInputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor(NUI_DATAPACKET_THROUGH_BLOBVECTOR_PACKET_DATA_TYPE);
descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

END_IMPLEMENT_DESCRIPTOR(nuiTUIOOutputModule)

START_EXPORT_MODULES(TUIOOutputModule)
REGISTER_MODULE(nuiTUIOOutputModule, "nuiGaussianBlurFilter", 1, 0)
END_EXPORT_MODULES()

#endif