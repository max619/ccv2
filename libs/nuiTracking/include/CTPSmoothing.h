#ifndef NUI_CTP_SMOOTHING
#define NUI_CTP_SMOOTHING

#include <opencv2\core.hpp>
#include "nuiTrackerStructs.h"

class CTPSmoothing
{
public:
	CTPSmoothing();
	~CTPSmoothing();

	void process(BlobVector* src);
	int maxDepth;
private:
	std::map<int, BlobVector*> vectors;
	void updateBlob(Blob* b, int iter, int frame);
	CvMat* calcMeasurmentsEq(Blob* b, int iter, int frame);
};

#endif
