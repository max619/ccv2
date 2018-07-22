#include "nuiDebugVideoSink.h"
#include <sstream>

MODULE_DECLARE(DebugVideoSink, "native", "Sink to display video");

nuiDebugVideoSink::nuiDebugVideoSink() : nuiModule(), D3D11WinApp(640,480, std::string("wnd")) {
	MODULE_INIT();

	this->input = new nuiEndpoint(this);
	this->input->setTypeDescriptor(std::string("IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->input);

	dispFrame = NULL;
}

nuiDebugVideoSink::~nuiDebugVideoSink()
{

}

void nuiDebugVideoSink::update()
{
	lockMutex();
	this->input->lock();
	void* data;
	nuiDataPacket* packet = this->input->getData();
	if (packet == NULL)
		return;
	packet->unpackData(data);
	IplImage* frame = (IplImage*)data;
	dispFrame = NULL;
	dispFrame = cvCloneImage(frame);
	m_frame_bgr = cvarrToMat(dispFrame);
	delete packet;
	this->input->unlock();
	unlockMutex();
	render();
}

void nuiDebugVideoSink::start()
{
	m_mode = MODE::MODE_GPU_RGBA;
	create();
	nuiModule::start();
	LOG(NUI_DEBUG, "starting video sink");
}