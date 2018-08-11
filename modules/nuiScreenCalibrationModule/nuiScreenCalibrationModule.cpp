#include "nuiScreenCalibrationModule.h"


MODULE_DECLARE(ScreenCalibrationModule, "native", "Calibrate blobs/IplImage to screen size");

NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_IPLIMAGE(ScreenCalibrationModuleIplImage)
NUI_DATAPACKET_DEFAULT_DEFENITION_THROUGH_BLOBVECTOR(ScreenCalibrationModuleBlobVector)


nuiScreenCalibrationModule::nuiScreenCalibrationModule() : nuiModule()
{
	MODULE_INIT();
	this->_pInput = new nuiEndpoint(this);
	this->_pInput->setTypeDescriptor(std::string("BlobVector,IplImage"));
	this->setInputEndpointCount(1);
	this->setInputEndpoint(0, this->_pInput);

	this->_pOutput0 = new nuiEndpoint(this);
	this->_pOutput0->setTypeDescriptor(std::string("BlobVector,IplImage"));
	this->setOutputEndpointCount(1);
	this->setOutputEndpoint(0, this->_pOutput0);

	this->_pOutputDataPacket0 = new nuiScreenCalibrationModuleBlobVectorDataPacket();
	this->_pOutputDataPacket1 = new nuiScreenCalibrationModuleIplImageDataPacket();

	homography = cvCreateMat(3, 3, CV_32F);

	LinkProperty("screenWidth", NUI_PROPERTY_INTEGER, destsize.width);
	LinkProperty("screenHeight", NUI_PROPERTY_INTEGER, destsize.height);
	LinkProperty("sx0", NUI_PROPERTY_FLOAT, screenpoints[0].x);
	LinkProperty("sy0", NUI_PROPERTY_FLOAT, screenpoints[0].y);
	LinkProperty("sx1", NUI_PROPERTY_FLOAT, screenpoints[1].x);
	LinkProperty("sy1", NUI_PROPERTY_FLOAT, screenpoints[1].y);
	LinkProperty("sx2", NUI_PROPERTY_FLOAT, screenpoints[2].x);
	LinkProperty("sy2", NUI_PROPERTY_FLOAT, screenpoints[2].y);
	LinkProperty("sx3", NUI_PROPERTY_FLOAT, screenpoints[3].x);
	LinkProperty("sy3", NUI_PROPERTY_FLOAT, screenpoints[3].y);
}


nuiScreenCalibrationModule::~nuiScreenCalibrationModule()
{
	delete this->_pOutput0;
	delete this->_pOutputDataPacket0;
	delete this->_pOutputDataPacket1;
	delete this->_pInput;

	delete screenpoints;
	cvReleaseMat(&homography);
}

void nuiScreenCalibrationModule::update()
{
	void* data;
	nuiDataPacket* packet = this->_pInput->getData();
	if (packet == NULL)
		return;
	packet->unpackData(data);
	if (data == NULL)
		return;

	if(strcmp(packet->getDataPacketType(), "IplImage") == 0)
	{
		this->_pOutput0->lock();
		this->_pOutput0->clear();
		IplImage* img = cvCloneImage((IplImage*)data);
		cvWarpPerspective(img, img, homography);

		this->_pOutputDataPacket1->packData(img);
		this->_pOutput0->setData(_pOutputDataPacket1);
		this->_pOutput0->transmitData();
		this->_pOutput0->unlock();

		cvReleaseImage(&img);
	}
	else if (strcmp(packet->getDataPacketType(), "BlobVector") == 0)
	{
		this->_pOutput0->lock();
		this->_pOutput0->clear();
		BlobVector* vec = cloneBlobVector((BlobVector*)data);
		float* hmat = reinterpret_cast<float*>(homography->data.fl);
		for (size_t i = 0; i < vec->size; i++)
		{
			Blob& b = vec->blobs[i];
			b.keyPoint.pt.x = hmat[0] * b.keyPoint.pt.x + hmat[1] * b.keyPoint.pt.y + hmat[2];
			b.keyPoint.pt.y = hmat[3] * b.keyPoint.pt.x + hmat[4] * b.keyPoint.pt.y + hmat[5];
			vec->blobs[i] = b;
		}
		vec->targetResolution.height = destsize.height;
		vec->targetResolution.width = destsize.width;

		this->_pOutputDataPacket0->packData(vec);
		this->_pOutput0->setData(_pOutputDataPacket0);
		this->_pOutput0->transmitData();
		this->_pOutput0->unlock();

		releaseBlobVector(vec);
	}

	if (packet->isLocalCopy())
		delete packet;

}

void nuiScreenCalibrationModule::start()
{
	cvGetPerspectiveTransform(screenpoints, dstpoints, homography);

	nuiModule::start();
}

void nuiScreenCalibrationModule::stop()
{
	nuiModule::stop();
}

void nuiScreenCalibrationModule::propertyUpdated(std::string& name, nuiProperty* prop, nuiLinkedProperty* linkedProp, void* userdata)
{
	if (name == "screenWidth" || name == "screenHeight")
	{
		dstpoints[0] = CvPoint2D32f(0, 0);
		dstpoints[1] = CvPoint2D32f(destsize.width, 0);
		dstpoints[2] = CvPoint2D32f(destsize.width, destsize.height);
		dstpoints[3] = CvPoint2D32f(0, destsize.height);
	}
}