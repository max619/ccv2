/**
* \file      blobStructs.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/


#ifndef NUI_TRACKER_STRUCTS
#define NUI_TRACKER_STRUCTS

#include <opencv2/core.hpp>

#define NUI_DATAPACKET_THROUGH_BLOBVECTOR_PACKET_DATA_TYPE "BlobVector"

#define NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_BLOBVECTOR(modulename)\
nui##modulename##DataPacket::nui##modulename##DataPacket() : nuiDataPacket()\
{\
	data = NULL;\
}\
\
nui##modulename##DataPacket::~nui##modulename##DataPacket()\
{\
	if (data != NULL)\
	{\
		BlobVector* vec = (BlobVector*)data;\
		releaseBlobVector(vec);\
		data = NULL;\
	}\
};\
\
nuiDataPacketError::err nui##modulename##DataPacket::packData(const void *_data)\
{\
	this->setLocalCopy(false);\
	this->data = (void*)_data;\
	return NUI_DATAPACKET_OK;\
};\
\
nuiDataPacketError::err nui##modulename##DataPacket::unpackData(void* &_data)\
{\
	_data = (void*)this->data;\
	return NUI_DATAPACKET_OK;\
};\
\
nuiDataPacket* nui##modulename##DataPacket::copyPacketData(nuiDataPacketError::err &errorCode)\
{\
	nui##modulename##DataPacket* newpacket = new nui##modulename##DataPacket();\
	newpacket->packData(cloneBlobVector((BlobVector*)data));\
	newpacket->setLocalCopy(true);\
	errorCode = nuiDataPacketError::NoError;\
	return newpacket;\
};\
\
char* nui##modulename##DataPacket::getDataPacketType()\
{\
	return NUI_DATAPACKET_THROUGH_BLOBVECTOR_PACKET_DATA_TYPE;\
};\

struct Blob
{
	long id;
	cv::KeyPoint keyPoint;
};

struct BlobVector
{
	BlobVector(size_t capacity);
	size_t size;
	size_t capacity;
	size_t removedBlobsSize;
	size_t newBlobsSize;
	size_t updatedBlobSize;
	CvSize targetResolution;
	Blob* blobs;
	Blob** removedBlobs;
	Blob** newBlobs;
	Blob** updatedBlob;
};

BlobVector* vecToArr(std::vector<Blob> vec);
BlobVector* vecToArr(std::vector<Blob> vec, size_t capacity);
void releaseBlobVector(BlobVector*& b);
BlobVector* cloneBlobVector(BlobVector* vec);


#endif // !NUI_TRACKER_STRUCTS


