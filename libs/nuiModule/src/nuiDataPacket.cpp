#include "nuiDataPacket.h"

nuiDataPacket::~nuiDataPacket(){ };

void nuiReleaseDataPacket(nuiDataPacket** packet)
{
	if(packet[0]->isLocalCopy())
		delete packet[0];
	packet[0] = nullptr;
}