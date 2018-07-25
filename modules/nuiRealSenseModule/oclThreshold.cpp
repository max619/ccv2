#include "oclThreshold.h"



oclThreshold::oclThreshold() : nuiOpenClAlgorithm(1)
{
	functionsNames = new const char*[1];
	functionsNames[0] = "calcThreshold";
	clmeminit = false;
}


oclThreshold::~oclThreshold()
{
	nuiOpenClAlgorithm::~nuiOpenClAlgorithm();

	_aligned_free(input);
	_aligned_free(output);
	clReleaseMemObject(inputMem);
	clReleaseMemObject(outputMem);
}

IplImage * oclThreshold::calcThreshold(uint16_t * src, uint width, uint height, uint16_t min, uint16_t max)
{
	if (!mutex.try_lock())
		return NULL;
#ifdef _DEBUG	
	bool queueProfilingEnable = true;
	if (queueProfilingEnable)
		QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif


	cl_int err = CL_SUCCESS;
	ocl_container* container = getOclFunctionPtr(0);
	if (!clmeminit)
		initMem(container, width, height);


	size_t origin[] = { 0, 0, 0 };
	size_t region[] = { width, height, 1 };
	size_t image_row_pitch;
	size_t image_slice_pitch;
	cl_int *inptPtr = (cl_int *)clEnqueueMapImage(container->commandQueue, inputMem, true, CL_MAP_WRITE, origin, region, &image_row_pitch, &image_slice_pitch, 0, NULL, NULL, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueMapBuffer returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}

	memcpy_s(input, inputSize, src, width * height * sizeof(uint16_t));

	err = clEnqueueUnmapMemObject(container->commandQueue, inputMem, inptPtr, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueUnmapMemObject returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}


	cl_ushort cl_max = max;
	cl_ushort cl_min = min;
	err = clSetKernelArg(container->kernel, 0, sizeof(cl_ushort), &cl_min);
	err = clSetKernelArg(container->kernel, 1, sizeof(cl_ushort), &cl_max);
	err = clSetKernelArg(container->kernel, 2, sizeof(cl_mem), &inputMem);
	err = clSetKernelArg(container->kernel, 3, sizeof(cl_mem), &outputMem);


#ifdef _DEBUG	
	if (queueProfilingEnable)
	{
		QueryPerformanceCounter(&performanceCountNDRangeStop);
		QueryPerformanceFrequency(&perfFrequency);
		LogInfo("Kerenel initialization took time %f ms.\n",
			1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
	}
#endif

#ifdef _DEBUG	
	if (queueProfilingEnable)
		QueryPerformanceCounter(&performanceCountNDRangeStart);
#endif
	err = ExecKernel(container, width, height);

	if (CL_SUCCESS != err)
	{
		LogError("Error: ExecKernel %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}
#ifdef _DEBUG	
	if (queueProfilingEnable)
	{
		QueryPerformanceCounter(&performanceCountNDRangeStop);
		QueryPerformanceFrequency(&perfFrequency);
		LogInfo("NDRange performance counter time %f ms.\n",
			1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
	}
#endif

	cl_int *resultPtr = (cl_int *)clEnqueueMapImage(container->commandQueue, outputMem, true, CL_MAP_READ, origin, region, &image_row_pitch, &image_slice_pitch, 0, NULL, NULL, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueMapBuffer returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}


	err = clEnqueueUnmapMemObject(container->commandQueue, outputMem, resultPtr, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueUnmapMemObject returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}

	IplImage* res = cvCreateImage(CvSize(width, height), IPL_DEPTH_8U, 1);
	memcpy_s(res->imageDataOrigin, width * height, output, width * height);



	mutex.unlock();
	return res;
}

int oclThreshold::initMem(ocl_container* container, int width, int height)
{

	cl_int err = CL_SUCCESS;

	inputSize = getAllignedBufferSize(width, height, sizeof(cl_uint16));
	input = (cl_uint16*)allocAlligned(inputSize);

	outputSize = getAllignedBufferSize(width, height, sizeof(cl_uint8));
	output = (cl_uint8*)allocAlligned(outputSize);


	cl_image_format format;
	cl_image_desc desc;

	// Define the image data-type and order -
	// one channel (R) with unit values
	format.image_channel_data_type = CL_UNSIGNED_INT16;
	format.image_channel_order = CL_R;

	// Define the image properties (descriptor)
	desc.image_type = CL_MEM_OBJECT_IMAGE2D;
	desc.image_width = width;
	desc.image_height = height;
	desc.image_depth = 0;
	desc.image_array_size = 1;
	desc.image_row_pitch = 0;
	desc.image_slice_pitch = 0;
	desc.num_mip_levels = 0;
	desc.num_samples = 0;
#ifdef CL_VERSION_2_0
	desc.mem_object = NULL;
#else
	desc.buffer = NULL;
#endif

	inputMem = clCreateImage(container->context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, &format, &desc, input, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateImage for srcA returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	format.image_channel_data_type = CL_UNSIGNED_INT8;

	outputMem = clCreateImage(container->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, &format, &desc, output, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateImage for srcA returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	clmeminit = true;

	return err;

}

char * oclThreshold::getSourceCode(int id)
{
	return "constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;__kernel void calcThreshold(unsigned short min, unsigned short max, read_only image2d_t imageA, write_only image2d_t imageC){const int x = get_global_id(0);const int y = get_global_id(1);unsigned short A = read_imageui(imageA, sampler, (int2)(x, y)).x; if(A > min && A < max) {write_imageui(imageC, (int2)(x, y), 255);} else {write_imageui(imageC, (int2)(x, y), 0);}}";
}

int oclThreshold::getFunctionsCount()
{
	return 1;
}
