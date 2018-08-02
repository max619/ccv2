#ifndef NUI_BLOB_TRACKER
#define NUI_BLOB_TRACKER

#include "nuiTrackerStructs.h"
#include <opencv2/features2d.hpp>

class blobTracker
{
public:
	blobTracker();
	~blobTracker();
	std::vector<Blob> trackBlobs(std::vector<Blob>& input);
	void setMaxDistance(float val);
private:
	bool isFirstFrameArrived;
	std::vector<Blob> prevBlobs;
	long idCounter;
	float maxDist, maxDistSq;

};

#endif // !NUI_BLOB_TRACKER




