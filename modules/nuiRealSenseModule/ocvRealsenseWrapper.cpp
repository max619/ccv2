#include "ocvRealsenseWrapper.h"



ocvRealsenseWrapper::ocvRealsenseWrapper()
{
	depthImage = NULL;
	colorImage = NULL;
}


ocvRealsenseWrapper::~ocvRealsenseWrapper()
{
	close();
}

bool ocvRealsenseWrapper::open(int index)
{
	try
	{
		pipe.start();
		opened = true;
	}
	catch (_exception ex)
	{

	}

	return opened;
}

bool ocvRealsenseWrapper::close()
{
	if (opened)
	{
		pipe.stop();
		opened = false;
	}

	return !opened;
}

bool ocvRealsenseWrapper::queryColorFrame(IplImage ** img)
{

	frameset = pipe.wait_for_frames();
	depthFrame = color_map(frameset);
	void* data_ptr = (void*)depthFrame.get_data();
	IplImage * image = *img;
	if (image == NULL)
	{
		image = new IplImage();
	}
}

bool ocvRealsenseWrapper::queryDepthFrame(IplImage ** img)
{
}
