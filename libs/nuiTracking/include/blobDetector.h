/**
* \file      blobDetector.h
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

#ifndef NUI_BLOB_DETECTOR
#define NUI_BLOB_DETECTOR


#include "nuiTrackerStructs.h"
#include <opencv2/features2d.hpp>

class blobDetector
{
public:
	blobDetector();
	blobDetector(cv::SimpleBlobDetector::Params& params);
	~blobDetector();

	std::vector<Blob> detectBlobs(IplImage* img);
	std::vector<Blob> detectBlobs(cv::Mat img);
private:
	cv::Ptr<cv::SimpleBlobDetector> detector;
};

#endif

