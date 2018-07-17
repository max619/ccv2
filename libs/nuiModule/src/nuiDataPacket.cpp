#include "nuiDataPacket.h"

nuiDataPacket::~nuiDataPacket(){ };

void nuiReleaseDataPacket(nuiDataPacket** packet)
{
	delete packet[0];
	packet[0] = nullptr;
}