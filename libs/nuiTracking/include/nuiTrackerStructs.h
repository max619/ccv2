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
#include <map>
#include <time.h>

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
		delete vec;\
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
	newpacket->packData(((BlobVector*)data)->clone());\
	newpacket->setLocalCopy(true);\
	errorCode = nuiDataPacketError::NoError;\
	return newpacket;\
};\
\
char* nui##modulename##DataPacket::getDataPacketType()\
{\
	return NUI_DATAPACKET_THROUGH_BLOBVECTOR_PACKET_DATA_TYPE;\
};\

enum BlobState
{
	NUI_BLOB_STATE_UNKNOWN,
	NUI_BLOB_STATE_PRE_CREATED,
	NUI_BLOB_STATE_CREATED,
	NUI_BLOB_STATE_UPDATED,
	NUI_BLOB_STATE_PRE_REMOVED,
	NUI_BLOB_STATE_REMOVED
};

struct Blob
{
	long id;
	cv::KeyPoint keyPoint;
	int createdAt, lastUpdatedAt, removedAt;
	BlobState state;
	Blob* clone();
	float velocity, avgVelocity, smoothCoeff;
	CvPoint2D32f vel, avel;
};

class BlobVector
{
public:
	BlobVector();
	~BlobVector();
public:
	CvSize targetResolution;
	void addBlob(Blob* b);
	/*void addUpdatedBlob(Blob* b);
	void addRemovedBlob(Blob* b);*/
	Blob* getBlob(int id);
	std::vector<Blob*> getNewBlobs();
	std::vector<Blob*> getUpdatedBlobs();
	std::vector<Blob*> getRemovedBlobs();
	std::vector<Blob*> getBlobByState(const BlobState state);
	std::map<int, Blob*>& getBlobs();
	void removeBlob(Blob* b);
	BlobVector* clone();
	Blob* findClosestBlob(Blob* b, float maxDist);
	int getFrame();
	void setFrame(int frame);
	clock_t getTime();
	void setTime(clock_t t);
private:
	int frame;
	std::map<int, Blob*> blobs;
	clock_t time;
};

//BlobVector* vecToArr(std::vector<Blob> vec);
//BlobVector* vecToArr(std::vector<Blob> vec, size_t capacity);
////void releaseBlobVector(BlobVector*& b);
//BlobVector* cloneBlobVector(BlobVector* vec);


#endif // !NUI_TRACKER_STRUCTS


