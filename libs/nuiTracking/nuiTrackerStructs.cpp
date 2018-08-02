#include "nuiTrackerStructs.h"

BlobVector* vecToArr(std::vector<Blob> vec)
{
	BlobVector* res = new BlobVector();
	
	res->blobs = (Blob*)malloc(sizeof(Blob) * vec.size());
	memcpy_s(res->blobs, sizeof(Blob) * vec.size(), vec.data(), sizeof(Blob) * vec.size());
	res->size = vec.size();
	return res;
}

void releaseBlobVector(BlobVector *& b)
{
	free(b->blobs);
	b = NULL;
}
