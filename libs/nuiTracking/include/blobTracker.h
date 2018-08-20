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
	int blobLive, minblobLive;
	float easeCoeff;
	bool applyEasingFunction;
private:
	bool isFirstFrameArrived;
	BlobVector* prevBlobs;
	long idCounter;
	float maxDist, maxDistSq;
	int frame;
	CvPoint2D32f applyEaseFunction(CvPoint2D32f new_pt, CvPoint2D32f old_pt, float coeff);

};

#endif // !NUI_BLOB_TRACKER




