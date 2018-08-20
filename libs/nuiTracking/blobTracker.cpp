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
	frame = 0;
	applyEasingFunction = false;
	easeCoeff = 1.f;
}


blobTracker::~blobTracker()
{
}

BlobVector* blobTracker::trackBlobs(std::vector<Blob>& input)
{
	clock_t curTime = clock();

	if (!isFirstFrameArrived)
	{
		BlobVector* vector = new BlobVector();
		size_t i = 0;
		for (std::vector<Blob>::iterator it = input.begin(); it < input.end(); it++)
		{
			(*it).id = idCounter;
			(*it).createdAt = frame;
			(*it).state = NUI_BLOB_STATE_PRE_CREATED;
			vector->addBlob((*it).clone());
			idCounter++;
		}
		vector->setTime(curTime);
		prevBlobs = vector;
		isFirstFrameArrived = true;
		return vector->clone();
	}

	std::vector<Blob> newBlobs;
	for (std::vector<Blob>::iterator it = input.begin(); it < input.end(); it++)
	{
		Blob* parentBlob = prevBlobs->findClosestBlob(&(*it), maxDistSq);
		if (parentBlob != NULL && frame - parentBlob->createdAt > minblobLive)
		{
			if (parentBlob->lastUpdatedAt == frame)
				continue;
			parentBlob->lastUpdatedAt = frame;
			if (parentBlob->state == NUI_BLOB_STATE_PRE_CREATED)
			{
				parentBlob->state = NUI_BLOB_STATE_CREATED;
				parentBlob->avel.x = 0;
				parentBlob->avel.y = 0;
				parentBlob->vel.x = 0;
				parentBlob->vel.y = 0;
				parentBlob->avgVelocity = 0;
				parentBlob->velocity = 0;
			}
			else if (parentBlob->state != NUI_BLOB_STATE_REMOVED)
				parentBlob->state = NUI_BLOB_STATE_UPDATED;
			CvPoint2D32f oldpt = parentBlob->keyPoint.pt;
			parentBlob->keyPoint = (*it).keyPoint;
			if (applyEasingFunction)
				parentBlob->keyPoint.pt = applyEaseFunction((*it).keyPoint.pt, oldpt, easeCoeff);
			float dx = parentBlob->keyPoint.pt.x - oldpt.x;
			float dy = parentBlob->keyPoint.pt.y - oldpt.y;
			float t = ((curTime - prevBlobs->getTime()) / 1000.f);
			float v = sqrtf(dx * dx + dy * dy) / t;
			float vx = dx / t;
			float vy = dy / t;
			parentBlob->avel.x = (parentBlob->avel.x + vx) / 2;
			parentBlob->avel.y = (parentBlob->avel.y + vy) / 2;
			parentBlob->vel.x = vx;
			parentBlob->vel.y = vy;
			parentBlob->avgVelocity = (parentBlob->avgVelocity + v) / 2;
			parentBlob->velocity = v;
		}
		else if (parentBlob == NULL)
		{
			newBlobs.push_back(*it);
		}
	}

	for (std::vector<Blob>::iterator it = newBlobs.begin(); it < newBlobs.end(); it++)
	{
		(*it).createdAt = frame;
		(*it).state = NUI_BLOB_STATE_PRE_CREATED;
		(*it).id = idCounter;
		idCounter++;
		prevBlobs->addBlob((*it).clone());
	}

	std::vector<Blob*> blobs = prevBlobs->getBlobByState(NUI_BLOB_STATE_PRE_CREATED);
	for (std::vector<Blob*>::iterator it = blobs.begin(); it < blobs.end(); it++)
	{
		if (frame - (*it)->createdAt > minblobLive)
		{
			prevBlobs->removeBlob((*it));
			delete (*it);
		}
	}

	blobs = prevBlobs->getBlobByState(NUI_BLOB_STATE_CREATED);
	for (std::vector<Blob*>::iterator it = blobs.begin(); it < blobs.end(); it++)
	{
		if (frame - (*it)->lastUpdatedAt > blobLive)
		{
			(*it)->state = NUI_BLOB_STATE_PRE_REMOVED;
			(*it)->removedAt = frame;
		}
	}

	blobs = prevBlobs->getBlobByState(NUI_BLOB_STATE_UPDATED);
	for (std::vector<Blob*>::iterator it = blobs.begin(); it < blobs.end(); it++)
	{
		if (frame - (*it)->lastUpdatedAt > blobLive)
		{
			(*it)->state = NUI_BLOB_STATE_PRE_REMOVED;
			(*it)->removedAt = frame;
		}
	}

	blobs = prevBlobs->getBlobByState(NUI_BLOB_STATE_REMOVED);
	for (std::vector<Blob*>::iterator it = blobs.begin(); it < blobs.end(); it++)
	{
		if (frame - (*it)->removedAt > blobLive)
		{
			prevBlobs->removeBlob((*it));
			delete (*it);
		}
	}

	blobs = prevBlobs->getBlobByState(NUI_BLOB_STATE_PRE_REMOVED);
	for (std::vector<Blob*>::iterator it = blobs.begin(); it < blobs.end(); it++)
	{
		if (frame - (*it)->removedAt > blobLive)
		{
			(*it)->state = NUI_BLOB_STATE_REMOVED;
		}
	}


	frame++;
	prevBlobs->setFrame(frame);
	prevBlobs->setTime(curTime);
	return prevBlobs->clone();
}

void blobTracker::setMaxDistance(float val)
{
	maxDist = val;
	maxDistSq = val * val;
}

CvPoint2D32f blobTracker::applyEaseFunction(CvPoint2D32f new_pt, CvPoint2D32f old_pt, float coeff)
{
	float dx = new_pt.x - old_pt.x;
	float dy = new_pt.y - old_pt.y;

	float d = dx * dx + dy * dy;

	float xc = 1 / (coeff * d);
	dx = dx * (1 - (xc > 1 ? 1 : xc));
	dy = dy * (1 - (xc > 1 ? 1 : xc));

	return CvPoint2D32f(old_pt.x + dx, old_pt.y + dy);
}
