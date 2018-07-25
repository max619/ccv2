

#include "oclDepthToWorld.h"



oclDepthToWorld::oclDepthToWorld() : nuiOpenClAlgorithm(2)
{
	functionsNames = new const char*[2];
	functionsNames[0] = "calcWorldCoordinatesNormal";
	functionsNames[1] = "calcWorldCoordinatesInverseBrownConrady";
	clmeminit = false;
}


oclDepthToWorld::~oclDepthToWorld()
{
	nuiOpenClAlgorithm::~nuiOpenClAlgorithm();

	_aligned_free(input);
	_aligned_free(output);
	clReleaseMemObject(inputMem);
	clReleaseMemObject(outputMem);
}

IplImage * oclDepthToWorld::calcWorldCoordinatesNormal(uint16_t * data, float scale, uint width, uint height, rs2_intrinsics intrisnic)
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

	memcpy_s(input, inputSize, data, width * height * sizeof(uint16_t));

	err = clEnqueueUnmapMemObject(container->commandQueue, inputMem, inptPtr, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueUnmapMemObject returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}


	err = clSetKernelArg(container->kernel, 0, sizeof(cl_float), &(intrisnic.ppx));
	err = clSetKernelArg(container->kernel, 1, sizeof(cl_float), &(intrisnic.ppy));
	err = clSetKernelArg(container->kernel, 2, sizeof(cl_float), &(intrisnic.fx));
	err = clSetKernelArg(container->kernel, 3, sizeof(cl_float), &(intrisnic.fy));
	err = clSetKernelArg(container->kernel, 4, sizeof(cl_float), &scale);
	err = clSetKernelArg(container->kernel, 5, sizeof(cl_mem), &inputMem);
	err = clSetKernelArg(container->kernel, 6, sizeof(cl_mem), &outputMem);


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

	IplImage* res = cvCreateImage(CvSize(width, height), IPL_DEPTH_32F, 4);
	memcpy_s(res->imageDataOrigin, width * height, output, width * height);



	mutex.unlock();
	return res;
}


IplImage * oclDepthToWorld::calcWorldCoordinatesInverseBrownConrady(uint16_t * data, float scale, uint width, uint height, rs2_intrinsics intrisnic)
{
	return nullptr;
}

int oclDepthToWorld::initMem(ocl_container * container, int width, int height)
{
	cl_int err = CL_SUCCESS;

	inputSize = getAllignedBufferSize(width, height, sizeof(cl_uint16));
	input = (cl_uint16*)allocAlligned(inputSize);

	outputSize = getAllignedBufferSize(width, height, sizeof(cl_float4));
	output = (cl_float4*)allocAlligned(outputSize);


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

	format.image_channel_data_type = CL_FLOAT;

	outputMem = clCreateImage(container->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, &format, &desc, output, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateImage for srcA returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	clmeminit = true;

	return err;

}

char * oclDepthToWorld::getSourceCode(int id)
{
	nuiFrameworkManager* ptr = static_cast<nuiFrameworkManager*>(getFrameworkPtr());
	char* source;
	size_t size;
	if (id == 0)
	{
		std::string str = ptr->getRelativeToStartupPath("modules\\ocl\\calcWorldCoordinatesNormal.cl");
		ReadSourceFromFile(str.c_str(), &source, &size);
		/*char* sourcecropped = new char[size];
		memcpy(sourcecropped, source, size);
		delete source;
		source = sourcecropped;*/
		return source;
	}
	else
		return "constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;__kernel void calcWorldCoordinatesInverseBrownConrady(float ppx, float ppy, float fx, float fy, float depthmul, read_only image2d_t imageA, write_only image2d_t imageC, float c0, float c1, float c2, float c3, float c4 ){const int x = get_global_id(0);const int y = get_global_id(1);float u = (x - ppx) / fx;float v = (y - ppy) / fy;float r2 = u * u + v * v;float f = 1 + c0 * r2 + c1 * r2*r2 + c4 * r2*r2*r2;float ux = x * f + 2 * c2*u*v + c3 * (r2 + 2 * x*x);float uy = y * f + 2 * c3*u*v + c2 * (r2 + 2 * x*x);u = ux;v = uy;float ddepth = (read_imageui(imageA, sampler, (int2)(x, y)).x) * depthmul;write_imagef(imageC, (int2)(x, y), float3(ddepth * u, ddepth * v, ddepth));}";
}

int oclDepthToWorld::getFunctionsCount()
{
	return 1;
}

