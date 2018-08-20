/**
* \file      blobDetector.cpp
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

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
		b.avgVelocity = 0;
		b.velocity = 0;
		b.avel.x = 0;
		b.avel.y = 0;
		b.vel.x = 0;
		b.vel.y = 0;
	}

	return blobs;
}
