#ifndef NUI_TRACKER_STRUCTS
#define NUI_TRACKER_STRUCTS

#include <opencv2/core.hpp>

struct Blob
{
	long id;
	cv::KeyPoint keyPoint;
};

struct BlobVector
{
	Blob* blobs;
	size_t size;
	CvSize targetResolution;
};

BlobVector* vecToArr(std::vector<Blob> vec);
void releaseBlobVector(BlobVector*& b);

#endif // !NUI_TRACKER_STRUCTS


