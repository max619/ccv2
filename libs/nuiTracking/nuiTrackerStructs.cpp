/**
* \file      nuiTrackerStructs.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "nuiTrackerStructs.h"

//BlobVector* vecToArr(std::vector<Blob> vec)
//{
//	return vecToArr(vec, vec.size());
//}
//
//BlobVector * vecToArr(std::vector<Blob> vec, size_t capacity)
//{
//	BlobVector* res = new BlobVector(capacity);
//	memcpy_s(res->blobs, sizeof(Blob) * vec.size(), vec.data(), sizeof(Blob) * vec.size());
//	res->size = vec.size();
//	return res;
//}


BlobVector::BlobVector()
{
	
}

BlobVector::~BlobVector()
{
	for (std::map<int, Blob*>::iterator it = blobs.begin(); it != blobs.end(); it++)
	{
		delete it->second;
	}
}

void BlobVector::addBlob(Blob* b)
{
	blobs.emplace(b->id, b);
}

Blob * BlobVector::getBlob(int id)
{
	std::map<int, Blob*>::iterator it = blobs.find(id);
	return it != blobs.end() ? it->second : NULL;
}

std::vector<Blob*> BlobVector::getNewBlobs()
{
	return getBlobByState(NUI_BLOB_STATE_CREATED);
}

std::vector<Blob*> BlobVector::getUpdatedBlobs()
{
	return getBlobByState(NUI_BLOB_STATE_UPDATED);
}

std::vector<Blob*> BlobVector::getRemovedBlobs()
{
	return getBlobByState(NUI_BLOB_STATE_REMOVED);
}

std::vector<Blob*> BlobVector::getBlobByState(const BlobState state)
{
	std::vector<Blob*> res = std::vector<Blob*>();
	for (std::map<int, Blob*>::iterator it = blobs.begin(); it != blobs.end(); it++)
	{
		if (it->second->state == state)
			res.push_back(it->second);
	}
	return res;
}

std::map<int, Blob*>& BlobVector::getBlobs()
{
	return blobs;
}

void BlobVector::removeBlob(Blob * b)
{
	blobs.erase(b->id);
}

BlobVector * BlobVector::clone()
{
	BlobVector* res = new BlobVector();
	
	for (std::map<int, Blob*>::iterator it = this->blobs.begin(); it != this->blobs.end(); it++)
	{
		Blob* b = it->second;
		if (b != NULL)
		{
			res->addBlob(b->clone());
		}
	}

	res->frame = this->frame;
	res->targetResolution = this->targetResolution;
	return res;
}

Blob * BlobVector::findClosestBlob(Blob* b, float maxDist)
{
	float min = 99999999.f;
	Blob* rb = NULL;
	for (std::map<int, Blob*>::iterator it = blobs.begin(); it != blobs.end(); it++)
	{
		Blob* _b = it->second;
		CvPoint p = _b->keyPoint.pt - b->keyPoint.pt;
		float sqDist = p.x * p.x + p.y * p.y;
		if (sqDist < maxDist && sqDist < min)
		{
			min = sqDist;
			rb = _b;
		}
	}

	return rb;
}

int BlobVector::getFrame()
{
	return frame;
}

void BlobVector::setFrame(int frame)
{
	this->frame = frame;
}

clock_t BlobVector::getTime()
{
	return time;
}

void BlobVector::setTime(clock_t t)
{
	this->time = t;
}

Blob * Blob::clone()
{
	Blob* b = (Blob*)malloc(sizeof(Blob));
	memcpy_s(b, sizeof(Blob), this, sizeof(Blob));
	return b;
}
