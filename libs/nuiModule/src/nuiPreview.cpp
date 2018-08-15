/**
* \file      nuiPreview.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

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
