
#include "stdafx.h"
#include "nuiIntDataPacket.h"



nuiIntDataPacket::nuiIntDataPacket() : nuiDataPacket()
{
	data = NULL;
};
nuiIntDataPacket::~nuiIntDataPacket()
{
	if (isLocalCopy())
	{
		delete data;
	}
};
nuiDataPacketError::err nuiIntDataPacket::packData(const void *_data)
{
	this->setLocalCopy(false);
	this->data = (int*)_data;
	return nuiDataPacketError::NoError;
};
nuiDataPacketError::err nuiIntDataPacket::unpackData(void* &_data)
{
	_data = (void*)this->data;
	return nuiDataPacketError::NoError;
};
nuiDataPacket* nuiIntDataPacket::copyPacketData(nuiDataPacketError::err &errorCode)
{
	nuiIntDataPacket* newDataPacket = new nuiIntDataPacket();

	int* val = new int();
	*val = *data;

	newDataPacket->packData(val);
	newDataPacket->setLocalCopy(true);

	errorCode = nuiDataPacketError::NoError;
	return newDataPacket;
};
char* nuiIntDataPacket::getDataPacketType()
{
	return "int";
};