#include "stdafx.h"


#ifndef NUI_TEST_MODULE
#define NUI_TEST_MODULE
#include "nuiModule.h"
#include "nuiPlugin.h"
#include "nuiDataPacket.h"
#include "nuiDataStream.h"
#include "nuiEndpoint.h"
#include "nuiDebugLogger.h"
class nuiTestModule : public nuiModule
{
public:
	nuiTestModule();
	~nuiTestModule();

	void update();
	void start();
	void stop();

	void setIncremental(int& i);

private:
	int* i;


	MODULE_INTERNALS();
};

#endif // !NUI_TEST_MODULE



