#include "blobTracker.h"



blobTracker::blobTracker()
{
	isFirstFrameArrived = false;
	idCounter = 0;
}


blobTracker::~blobTracker()
{
}

std::vector<Blob> blobTracker::trackBlobs(std::vector<Blob>& input)
{
	if (!isFirstFrameArrived)
	{
		for (std::vector<Blob>::iterator it = input.begin(); it < input.end(); it++)
		{
			(*it).id = idCounter;
			idCounter++;
		}

		prevBlobs = input;
		isFirstFrameArrived = true;
		return input;
	}

	for (std::vector<Blob>::iterator it = input.begin(); it < input.end(); it++)
	{
		float dist = 10000.f;
		std::vector<Blob>::iterator min;
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
			}
		}

		if (dist != 10000.f)
		{
			(*it).id = (*min).id;
		}
		else
		{
			(*it).id = idCounter;
			idCounter++;
		}
	}

	prevBlobs = input;

	return input;
}

void blobTracker::setMaxDistance(float val)
{
	maxDist = val;
	maxDistSq = val * val;
}
