#include "ocl3DPointCloudRotation.h"



ocl3DPointCloudRotation::ocl3DPointCloudRotation() : nuiOpenClAlgorithm(1)
{
	functionsNames = new const char*[1];
	functionsNames[0] = "rotateWorld";
	clmeminit = false;
}


ocl3DPointCloudRotation::~ocl3DPointCloudRotation()
{
	nuiOpenClAlgorithm::~nuiOpenClAlgorithm();

	_aligned_free(input);
	_aligned_free(output);
	clReleaseMemObject(inputMem);
	clReleaseMemObject(outputMem);
}

IplImage * ocl3DPointCloudRotation::rotate(IplImage * src, Eigen::Quaternion<float> quaterninon)
{
	if (!mutex.try_lock())
		return NULL;
#ifdef ALLOW_BENCHMARKING	
	benchmark.startBenchmarking();
#endif

	int width = src->width;
	int height = src->height;


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

	memcpy_s(input, inputSize, src->imageData, width * height * sizeof(float) * 4);

	err = clEnqueueUnmapMemObject(container->commandQueue, inputMem, inptPtr, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueUnmapMemObject returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}

	cl_float4 quat = castQuaternionToFloat4(quaterninon);


	err = clSetKernelArg(container->kernel, 0, sizeof(cl_float4), &(quat));
	err = clSetKernelArg(container->kernel, 1, sizeof(cl_mem), &inputMem);
	err = clSetKernelArg(container->kernel, 2, sizeof(cl_mem), &outputMem);


#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("ocl3DPointCloudRotation::rotate memory initialization");
#endif

#ifdef ALLOW_BENCHMARKING	
	benchmark.startBenchmarking();
#endif
	err = ExecKernel(container, width, height);

	if (CL_SUCCESS != err)
	{
		LogError("Error: ExecKernel %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return nullptr;
	}
#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("ocl3DPointCloudRotation::rotate kernel processing");
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


int ocl3DPointCloudRotation::initMem(ocl_container * container, int width, int height)
{
	cl_int err = CL_SUCCESS;

	inputSize = getAllignedBufferSize(width, height, sizeof(cl_float4));
	input = (cl_float4*)allocAlligned(inputSize);

	outputSize = getAllignedBufferSize(width, height, sizeof(cl_float4));
	output = (cl_float4*)allocAlligned(outputSize);


	cl_image_format format;
	cl_image_desc desc;

	// Define the image data-type and order -
	// one channel (R) with unit values
	format.image_channel_data_type = CL_FLOAT;
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

	outputMem = clCreateImage(container->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, &format, &desc, output, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateImage for srcA returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	clmeminit = true;

	return err;

}

char * ocl3DPointCloudRotation::getSourceCode(int id)
{
	nuiFrameworkManager* ptr = static_cast<nuiFrameworkManager*>(getFrameworkPtr());
	char* source;
	size_t size;
	if (id == 0)
	{
		std::string str = ptr->getRelativeToStartupPath("modules\\ocl\\rotate.cl");
		ReadSourceFromFile(str.c_str(), &source, &size);
		return source;
	}
	else
		return "constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;__kernel void calcWorldCoordinatesInverseBrownConrady(float ppx, float ppy, float fx, float fy, float depthmul, read_only image2d_t imageA, write_only image2d_t imageC, float c0, float c1, float c2, float c3, float c4 ){const int x = get_global_id(0);const int y = get_global_id(1);float u = (x - ppx) / fx;float v = (y - ppy) / fy;float r2 = u * u + v * v;float f = 1 + c0 * r2 + c1 * r2*r2 + c4 * r2*r2*r2;float ux = x * f + 2 * c2*u*v + c3 * (r2 + 2 * x*x);float uy = y * f + 2 * c3*u*v + c2 * (r2 + 2 * x*x);u = ux;v = uy;float ddepth = (read_imageui(imageA, sampler, (int2)(x, y)).x) * depthmul;write_imagef(imageC, (int2)(x, y), float3(ddepth * u, ddepth * v, ddepth));}";
}

int ocl3DPointCloudRotation::getFunctionsCount()
{
	return 1;
}
