/**
* \file      blobTracker.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef NUI_BLOB_TRACKER
#define NUI_BLOB_TRACKER

#include "nuiTrackerStructs.h"
#include <opencv2/features2d.hpp>

class blobTracker
{
public:
	blobTracker();
	~blobTracker();
	BlobVector* trackBlobs(std::vector<Blob>& input);
	void setMaxDistance(float val);
private:
	bool isFirstFrameArrived;
	std::vector<Blob> prevBlobs;
	long idCounter;
	float maxDist, maxDistSq;

};

#endif // !NUI_BLOB_TRACKER




