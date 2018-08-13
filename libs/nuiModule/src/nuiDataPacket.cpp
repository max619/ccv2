#include "nuiDataPacket.h"

void nuiReleaseDataPacket(nuiDataPacket** packet)
{
	nuiDataPacket* pack = *packet;
	packet = nullptr;
	delete pack;
}