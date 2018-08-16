/**
 * \file      nuiEndpoint.h
 * \author    Anatoly Churikov
 * \author    Anatoly Lushnikov
 * \author    Maxim Bagryantsev
 * \date      2012-2018
 * \copyright Copyright 2012 NUI Group. All rights reserved.
 * \copyright Copyright 2018 Appollo Pro. All rights reserved.
 */

#ifndef NUI_ENDPOINT_H
#define NUI_ENDPOINT_H

#include <string>
#include <vector>
#include <map>

#ifndef PT_PASYNC
#define PT_PASYNC

#include "pasync.h"

#endif
#include "nuiUtils.h"
#include "nuiThread.h"
#include "nuiDataStream.h"
#include "nuiDataPacket.h"

class nuiModule;
class nuiModuleDescriptor;

//! endpoint descriptor
class nuiEndpointDescriptor
{
public:
	nuiEndpointDescriptor(std::string typeDescriptor);
	std::string &getDescriptor();
	void setDescriptor(std::string typeDescriptor);
	int getIndex();
	void setIndex(int index);
	void setParentModuleDescriptor(nuiModuleDescriptor* parentModuleDescriptor);
	nuiModuleDescriptor* getParentModuleDescriptor();

private:
	std::string typeDescriptor;
	nuiModuleDescriptor* parentModuleDescriptor;
	int index;
};

//! endpoint class, used to hold input and output data for module.
class nuiEndpoint
{
public:
	nuiEndpoint(nuiModule *hostModule);
	virtual ~nuiEndpoint();

	//! lock data and send it to all outgoing streams
	void transmitData();

	nuiEndpointDescriptor* getEndpointDescriptor();
	nuiDataStream* addConnection(nuiEndpoint *endpoint);
	nuiDatastreamError::err removeConnection(nuiEndpoint *endpoint);

	//! erase all connections at once
	void removeConnections();

	nuiDataStream* getDataStreamForEndpoint(nuiEndpoint *endpoint);
	unsigned int getConnectionCount();

	//! \todo setTypeDescriptor marked as public?
	void setTypeDescriptor(std::string typeDescriptor);

	//! set endpoint to hold provided data
	void setData(nuiDataPacket *dataPacket);
	//! obtain stored data
	nuiDataPacket* getData();

	//! assign parent module
	void setModuleHoster(nuiModule *moduleHoster);
	//! get parent module
	nuiModule* getModuleHoster();

	//! set endpoint to hold dataPacket and notify hoster module
	void writeData(nuiDataPacket *dataPacket);

	nuiEndpoint* getConnectedEndpointAtIndex(int index);
	inline std::string getTypeDescriptor();
	void lock();
	void unlock();
	void clear();

private:
	bool canBePairedWithEndpoint(nuiEndpoint *endpoint);
	bool canBeSettedData(nuiDataPacket *dataPacket);

	//! add endpoint and datastream, that connects this and that endpoints
	std::map<nuiEndpoint*, nuiDataStream*> dataStreams;
	pt::mutex *mtx;
	std::string typeDescriptor;
	nuiDataPacket *dataPacket;
	nuiModule* moduleHoster;
	nuiEndpointDescriptor* descriptor;

	//friend class nuiDataStream;
};

#endif//NUI_ENDPOINT_H