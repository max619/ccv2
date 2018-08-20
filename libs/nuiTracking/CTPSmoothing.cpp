#include "CTPSmoothing.h"



CTPSmoothing::CTPSmoothing()
{
	maxDepth = 20;
}


CTPSmoothing::~CTPSmoothing()
{
}

void CTPSmoothing::process(BlobVector * src)
{
	BlobVector* _src = src->clone();
	vectors.emplace(_src->getFrame(), _src);
}

void CTPSmoothing::updateBlob(Blob * b, int iter, int frame)
{
	
}

CvMat* CTPSmoothing::calcMeasurmentsEq(Blob * b, int iter, int frame)
{
	if (iter >= maxDepth)
		return NULL;

	std::map<int, BlobVector*>::iterator it = vectors.find(frame);
	if (it != vectors.end())
	{
		CvMat* res = cvCreateMat(4, 1, CV_32F);
		Blob* oldBlob = it->second->getBlob(b->id);

	}

}

