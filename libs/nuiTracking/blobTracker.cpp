/**
* \file      blobTracker.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#include "blobTracker.h"



blobTracker::blobTracker()
{
	isFirstFrameArrived = false;
	idCounter = 0;
}


blobTracker::~blobTracker()
{
}

BlobVector* blobTracker::trackBlobs(std::vector<Blob>& input)
{
	if (!isFirstFrameArrived)
	{
		BlobVector* vector = vecToArr(input);
		size_t i = 0;
		for (std::vector<Blob>::iterator it = input.begin(); it < input.end(); it++)
		{
			(*it).id = idCounter;
			idCounter++; 
			vector->newBlobs[i] = &(vector->blobs[i]);
			vector->newBlobsSize += 1;
			i++;
		}

		prevBlobs = input;
		isFirstFrameArrived = true;
		
		return vector;

	}	

	BlobVector* vector = vecToArr(input, input.size() + prevBlobs.size());
	std::vector<int> referenced;
	for (size_t i = 0; i < prevBlobs.size(); i++)
		referenced.push_back(0);
	size_t i = 0;
	for (std::vector<Blob>::iterator it = input.begin(); it < input.end(); it++)
	{
		float dist = 10000.f;
		std::vector<Blob>::iterator min;
		size_t j = 0, dst = -1;
		for (std::vector<Blob>::iterator pit = prevBlobs.begin(); pit < prevBlobs.end(); pit++)
		{
			cv::Point2f prevpt = (*pit).keyPoint.pt;
			cv::Point2f newpt = (*it).keyPoint.pt;

			cv::Point2f dstVec = newpt - prevpt;
			float sqdist = dstVec.x * dstVec.x + dstVec.y * dstVec.y;

			if (sqdist < maxDistSq && sqdist < dist)
			{
				dist = sqdist;
				min = pit;
				dst = j;
			}
			j++;
		}

		if (dist != 10000.f)
		{
			(*it).id = (*min).id;
			vector->blobs[i] = (*it);
			vector->updatedBlob[vector->updatedBlobSize] = &(vector->blobs[i]);
			vector->updatedBlobSize += 1;
		}
		else
		{
			(*it).id = idCounter;
			vector->blobs[i] = (*it);
			vector->newBlobs[vector->newBlobsSize] = &(vector->blobs[i]);
			vector->newBlobsSize += 1;
			idCounter++;
		}		
		if (dst != -1)
		{
			referenced.at(dst) += 1;
		}
		i++;
	}
	
	for (size_t i = 0; i < referenced.size(); i++)
	{
		int refs = referenced.at(i);
		if (refs == 0)
		{
			vector->blobs[vector->size] = prevBlobs.at(i);
			vector->removedBlobs[vector->removedBlobsSize] = &(vector->blobs[vector->size]);
			vector->removedBlobsSize += 1;
			vector->size += 1;
		}
	}

	prevBlobs = input;

	return vector;
}

void blobTracker::setMaxDistance(float val)
{
	maxDist = val;
	maxDistSq = val * val;
}
