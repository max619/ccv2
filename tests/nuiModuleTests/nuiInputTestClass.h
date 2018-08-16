#pragma once
#include "stdafx.h"
#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
#include "nuiIntDataPacket.h"

class nuiInputTestClass : public nuiModule
{
public:
	nuiInputTestClass();
	~nuiInputTestClass();

	void update();
	void setIncremental(int& i);

private:
	nuiEndpoint * inputEndpoint;
	int* i;

	MODULE_INTERNALS();
};

