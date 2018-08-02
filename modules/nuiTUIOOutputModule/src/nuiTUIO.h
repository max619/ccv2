//////////////////////////////////////////////////////////////////////////
// Name:		modules/nuiTUIOOutputModule
// Purpose:		Output TUIO
// Author:		Anatoly Lushnikov
// Copyright:	(c) 2012 NUI Group
//////////////////////////////////////////////////////////////////////////

#ifndef NUI_TUIOOUTPUT_MODULE
#define NUI_TUIOOUTPUT_MODULE

#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"

NUI_DATAPACKET_DEFAULT_DECLARATION(TUIOOutput, void*)

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

	// TUIO broadcast params
	char*			host;
	int				port;
	int				flashport;

	// TUIO class for broadcasting

	MODULE_INTERNALS();
};

IMPLEMENT_ALLOCATOR(nuiTUIOOutputModule)
IMPLEMENT_DEALLOCATOR(nuiTUIOOutputModule)

START_IMPLEMENT_DESCRIPTOR(nuiTUIOOutputModule, "native", "Output TUIO")

descriptor->setInputEndpointsCount(1);
nuiEndpointDescriptor* inputDescriptor = new nuiEndpointDescriptor("std::map<int, Blob>");
descriptor->addInputEndpointDescriptor(inputDescriptor, 0);

END_IMPLEMENT_DESCRIPTOR(nuiTUIOOutputModule)

START_EXPORT_MODULES(TUIOOutputModule)
REGISTER_MODULE(nuiTUIOOutputModule, "nuiGaussianBlurFilter", 1, 0)
END_EXPORT_MODULES()

#endif