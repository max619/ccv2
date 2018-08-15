/**
* \file      nuiTrackerStructs.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiTrackerStructs.h"

BlobVector* vecToArr(std::vector<Blob> vec)
{
	return vecToArr(vec, vec.size());
}

BlobVector * vecToArr(std::vector<Blob> vec, size_t capacity)
{
	BlobVector* res = new BlobVector(capacity);
	memcpy_s(res->blobs, sizeof(Blob) * vec.size(), vec.data(), sizeof(Blob) * vec.size());
	res->size = vec.size();
	return res;
}

void releaseBlobVector(BlobVector *& b)
{
	free(b->blobs);
	free(b->newBlobs);
	free(b->removedBlobs);
	free(b->updatedBlob);
	delete b;
	b = NULL;
}

BlobVector* cloneBlobVector(BlobVector* vec)
{
	BlobVector* res = new BlobVector(vec->size);
	if (vec->size != 0)
	{
		memcpy_s(res->blobs, sizeof(Blob) * vec->size, vec->blobs, sizeof(Blob) * vec->size);
	}
	res->size = vec->size;
	res->newBlobsSize = vec->newBlobsSize;
	res->removedBlobsSize = vec->removedBlobsSize;
	res->updatedBlobSize = vec->updatedBlobSize;
	res->targetResolution = vec->targetResolution;
	

	for (size_t i = 0; i < res->newBlobsSize; i++)
	{
		res->newBlobs[i] = reinterpret_cast<Blob*>((size_t)res->blobs +  (size_t)vec->newBlobs[i] - (size_t)vec->blobs);
	}
	for (size_t i = 0; i < res->removedBlobsSize; i++)
	{
		res->removedBlobs[i] = reinterpret_cast<Blob*>((size_t)res->blobs +  (size_t)vec->removedBlobs[i] - (size_t)vec->blobs );
	}
	for (size_t i = 0; i < res->updatedBlobSize; i++)
	{
		res->updatedBlob[i] = reinterpret_cast<Blob*>((size_t)res->blobs + (size_t)vec->updatedBlob[i] - (size_t)vec->blobs);
	}


	return res;
}

BlobVector::BlobVector(size_t cap)
{
	if (cap > 0)
	{
		blobs = (Blob*)malloc(sizeof(Blob)*cap);
		newBlobs = (Blob**)malloc(sizeof(void*)*cap);
		removedBlobs = (Blob**)malloc(sizeof(void*)*cap);
		updatedBlob = (Blob**)malloc(sizeof(void*)*cap);
	}
	else
	{
		blobs = NULL;
		newBlobs = NULL;
		removedBlobs = NULL;
		updatedBlob = NULL;
	}

	
	capacity = cap;
	updatedBlobSize = 0;
	newBlobsSize = 0;
	removedBlobsSize = 0;
	size = 0;
}
