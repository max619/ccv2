/**
 * \file      nuiDataPacket.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \date      2012-2013
 * \copyright Copyright 2011 NUI Group. All rights reserved.
 */

#ifndef NUI_DATA_PACKET
#define NUI_DATA_PACKET

#define NUI_DATAPACKET_OK nuiDataPacketError::NoError
#define NUI_DATAPACKET_ERROR nuiDataPacketError::Error

 //! namespaced enum for errors that Datapacket can produce
struct nuiDataPacketError
{
	enum err
	{
		NoError,
		Error,
	};
};

//! macro for default datapacket skeleton implementation
#define NUI_DATAPACKET_DEFAULT_DECLARATION(moduleName, datatype) \
class nui##moduleName##DataPacket : public nuiDataPacket \
{ \
public: \
	nui##moduleName##DataPacket(); \
	~nui##moduleName##DataPacket(); \
	nuiDataPacketError::err packData(const void *data); \
	nuiDataPacketError::err unpackData(void* &data); \
	nuiDataPacket* copyPacketData(nuiDataPacketError::err &errorCode); \
	char* getDataPacketType(); \
private: \
	datatype## data; \
}; \

#define NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(moduleName) \
nui##moduleName##DataPacket::nui##moduleName##DataPacket() : nuiDataPacket()\
{\
	data = NULL;\
};\
nui##moduleName##DataPacket::~nui##moduleName##DataPacket()\
{\
	if(isLocalCopy())\
	{\
		IplImage* img = (IplImage*)data; \
		cvReleaseImage(&img); \
		data = NULL; \
	}\
};\
nuiDataPacketError::err nui##moduleName##DataPacket::packData(const void *_data)\
{\
	this->setLocalCopy(false);\
	this->data = (IplImage*)_data;\
	return nuiDataPacketError::NoError;\
};\
nuiDataPacketError::err nui##moduleName##DataPacket::unpackData(void* &_data)\
{\
	_data = (void*)this->data;\
	return nuiDataPacketError::NoError;\
};\
nuiDataPacket* nui##moduleName##DataPacket::copyPacketData(nuiDataPacketError::err &errorCode)\
{\
	nui##moduleName##DataPacket* newDataPacket = new nui##moduleName##DataPacket();\
\
	IplImage* current = (IplImage*)this->data;\
	IplImage* newData = cvCloneImage(current);\
\
	newDataPacket->packData(newData);\
	newDataPacket->setLocalCopy(true);\
\
	errorCode = nuiDataPacketError::NoError;\
	return newDataPacket;\
};\
char* nui##moduleName##DataPacket::getDataPacketType()\
{\
	return "IplImage";\
};\

//! \class datapacket interface. 
//! DataPacket is data wrapper used to transfer data from one module to another
class nuiDataPacket
{
public:
	nuiDataPacket() { localCopy = false; };
	virtual ~nuiDataPacket() { };
	virtual nuiDataPacketError::err packData(const void *data) = 0;
	virtual nuiDataPacketError::err unpackData(void* &data) = 0;
	virtual nuiDataPacket* copyPacketData(nuiDataPacketError::err &errorCode) = 0;

	virtual char* getDataPacketType() = 0;

	//! checks whether data from one module should be copied or passed by reference
	virtual bool isLocalCopy() { return localCopy; };

	virtual void setLocalCopy(bool value) { localCopy = value; };
protected:
	bool localCopy;
};

void nuiReleaseDataPacket(nuiDataPacket** packet);


#endif// NUI_DATA_PACKET