#include <nuiPreview.h>


nuiPreview::nuiPreview()
{
	attached = false;
	sock = -1;

	mtx.lock();
}

nuiPreview::~nuiPreview()
{
}

bool nuiPreview::isAttached()
{
	return attached;
}

void nuiPreview::attachTo(int socket)
{
	this->sock = socket;
	attached = true;
	mtx.unlock();
}

void nuiPreview::detach()
{
	attached = false;
	sock = -1;
	mtx.lock();
}

void nuiPreview::sendData(imageData & data)
{
	if (!mtx.try_lock())
		return;

	mtx.unlock();
}
