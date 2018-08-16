#pragma once
#include "stdafx.h"
#include "nuiDataPacket.h"

class nuiIntDataPacket : public nuiDataPacket 
{ 
public: 
	nuiIntDataPacket(); 
	~nuiIntDataPacket(); 
	nuiDataPacketError::err packData(const void *data); 
	nuiDataPacketError::err unpackData(void* &data); 
	nuiDataPacket* copyPacketData(nuiDataPacketError::err &errorCode); 
	char* getDataPacketType(); 
private: 
	int* data; 
}; 



