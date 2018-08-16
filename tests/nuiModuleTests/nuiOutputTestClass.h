#pragma once
#include "stdafx.h"
#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "nuiIntDataPacket.h"

class nuiOutputTestClass : public nuiModule
{
public:
	nuiOutputTestClass();
	~nuiOutputTestClass();


	void update();
	void start();
	void stop();
	
private:
	int i;
	nuiEndpoint* outputEndpoint;
	nuiIntDataPacket* datapacket;

	MODULE_INTERNALS();
	
};

