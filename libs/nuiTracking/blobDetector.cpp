#include "blobDetector.h"



blobDetector::blobDetector()
{
	detector = cv::SimpleBlobDetector::create();	
}

blobDetector::blobDetector(cv::SimpleBlobDetector::Params& params)
{
	detector = cv::SimpleBlobDetector::create(params);
}


blobDetector::~blobDetector()
{
	detector.release();
}

std::vector<Blob> blobDetector::detectBlobs(IplImage * img)
{
	cv::Mat mat = cv::cvarrToMat(img);
	std::vector<Blob> res = detectBlobs(mat);
	mat.release();
	return res;
}

std::vector<Blob> blobDetector::detectBlobs(cv::Mat img)
{
	std::vector<Blob> blobs;
	std::vector<cv::KeyPoint> keypoints;
	detector->detect(img, keypoints);

	for (std::vector<cv::KeyPoint>::iterator it = keypoints.begin(); it < keypoints.end(); it++)
	{
		Blob b;
		b.id = -1;
		b.keyPoint = *it;
		blobs.push_back(b);
	}

	return blobs;
}
