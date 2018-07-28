#include "oclRSWorldProcessor.h"



oclRSWorldProcessor::oclRSWorldProcessor() : nuiOpenClAlgorithm(1)
{
	functionsNames = new const char*[1];
	functionsNames[0] = "processWorld";
	clmeminit = false;
}


oclRSWorldProcessor::~oclRSWorldProcessor()
{
	nuiOpenClAlgorithm::~nuiOpenClAlgorithm();

	_aligned_free(input);
	_aligned_free(output);
	clReleaseMemObject(inputMem);
	clReleaseMemObject(outputMem);
}

void oclRSWorldProcessor::processWorld(uint16_t * data, float& scale, rs2_intrinsics& intrisnic, Eigen::Quaternionf& quaterninon, IplImage* res)
{
	if (!mutex.try_lock())
		return;
#ifdef ALLOW_BENCHMARKING	
	benchmark.startBencmarking();
#endif

	int width = intrisnic.width;
	int height = intrisnic.height;

	cl_float16 depthprocess;
	depthprocess.s0 = intrisnic.fx;
	depthprocess.s1 = intrisnic.fy;
	depthprocess.s2 = intrisnic.ppx;
	depthprocess.s3 = intrisnic.ppy;
	depthprocess.s4 = scale;
	depthprocess.s5 = quaterninon.x();
	depthprocess.s6 = quaterninon.y();
	depthprocess.s7 = quaterninon.z();
	depthprocess.s8 = quaterninon.w();
	

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
		return;
	}

	memcpy_s(input, inputSize, data, width * height * sizeof(uint16_t));

	err = clEnqueueUnmapMemObject(container->commandQueue, inputMem, inptPtr, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueUnmapMemObject returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return;
	}


	err = clSetKernelArg(container->kernel, 0, sizeof(cl_float16), &depthprocess);
	err = clSetKernelArg(container->kernel, 1, sizeof(cl_mem), &inputMem);
	err = clSetKernelArg(container->kernel, 2, sizeof(cl_mem), &outputMem);


#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("oclRSWorldProcessor::processWorld kernel initializing");
#endif

#ifdef ALLOW_BENCHMARKING	
	benchmark.startBencmarking();
#endif
	err = ExecKernel(container, width, height);

	if (CL_SUCCESS != err)
	{
		LogError("Error: ExecKernel %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return;
	}
#ifdef ALLOW_BENCHMARKING	
	benchmark.stopBenchmarking("oclRSWorldProcessor::processWorld kernel processing");
#endif

	cl_int *resultPtr = (cl_int *)clEnqueueMapBuffer(container->commandQueue, outputMem, true, CL_MAP_READ, 0, width * height * sizeof(float) * 4, 0, NULL, NULL, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueMapBuffer returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return;
	}

	memcpy_s(res->imageDataOrigin, width * height * sizeof(float) * 4, output, width * height * sizeof(float) * 4);

	err = clEnqueueUnmapMemObject(container->commandQueue, outputMem, resultPtr, 0, NULL, NULL);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clEnqueueUnmapMemObject returned %s\n", TranslateOpenCLError(err));
		mutex.unlock();
		return;
	}


	mutex.unlock();
}

void oclRSWorldProcessor::processWorldCpu(uint16_t * data, float & scale, rs2_intrinsics & intrisnic, Eigen::Quaternionf & quaterninon, IplImage * res)
{
	cl_float4 quat = castQuaternionToFloat4(quaterninon);
#ifdef _OPENMP
#pragma omp parallel for
#endif	
	for (int y = 0; y < intrisnic.height; y++)
	{
		float* res_ptr = reinterpret_cast<float*>(res->imageData + (y * intrisnic.width * sizeof(float) * 4));
		uint16_t* src_ptr = reinterpret_cast<uint16_t*>((uint16_t*)data + (y * intrisnic.width * sizeof(unsigned char)));
		cl_float4 fmul, res;
		float x1, y1, z1, w, u, v, ddepth;
		for (int x = 0; x < intrisnic.width; x++, res_ptr++, src_ptr++)
		{
			u = (x - intrisnic.ppx) / intrisnic.fx;
			v = (y - intrisnic.ppy) / intrisnic.fy;
			ddepth = *src_ptr * scale;

			x1 = u * ddepth;
			y1 = v * ddepth;
			z1 = ddepth;
			w = 0;

			fmul.w = w * quat.w - x1 * quat.x - y1 * quat.y - z1 * quat.z;
			fmul.x = w * quat.x + x1 * quat.w + y1 * quat.z + z1 * quat.y;
			fmul.y = w * quat.y + y1 * quat.w + z1 * quat.x + x1 * quat.z;
			fmul.z = w * quat.z + z1 * quat.w + x1 * quat.y + y1 * quat.x;

			res.w = fmul.w * quat.w + fmul.x * quat.x + fmul.y * quat.y + fmul.z * quat.z;
			res.x = -fmul.w * quat.x + fmul.x * quat.w - fmul.y * quat.z - fmul.z * quat.y;
			res.y = -fmul.w * quat.y + fmul.y * quat.w - fmul.z * quat.x - fmul.x * quat.z;
			res.z = -fmul.w * quat.z + fmul.z * quat.w - fmul.x * quat.y - fmul.y * quat.x;

			*res_ptr = res.x;
			res_ptr++;
			*res_ptr = res.y;
			res_ptr++;
			*res_ptr = res.z;
			res_ptr++;
			*res_ptr = res.w;
		}
	}
}

void oclRSWorldProcessor::getTouchedPointsCpu(uint16_t* data, float& scale, rs2_intrinsics& intrisnic, Eigen::Vector3f& normal, Eigen::Vector3f& basept, Eigen::Vector2f& threshold, IplImage* res)
{	
#ifdef _OPENMP
#pragma omp parallel for
#endif	
	for (int y = 0; y < intrisnic.height; y++)
	{
		unsigned char* res_ptr = reinterpret_cast<unsigned char*>(res->imageData + (y * intrisnic.width * sizeof(unsigned char)));
		uint16_t* src_ptr = reinterpret_cast<uint16_t*>((uint16_t*)data + (y * intrisnic.width * sizeof(unsigned char)));
		cl_float4 fmul, res;
		float u, v, ddepth, dist;
		Eigen::Vector3f respt;
		for (int x = 0; x < intrisnic.width; x++, res_ptr++, src_ptr++)
		{
			u = (x - intrisnic.ppx) / intrisnic.fx;
			v = (y - intrisnic.ppy) / intrisnic.fy;
			ddepth = *src_ptr * scale;

			respt.x() = u * ddepth;
			respt.y() = v * ddepth;
			respt.z() = ddepth;

			respt = respt - basept;
			float dot = respt.dot(normal);

			if (dot > threshold.x() && dot < threshold.y())
			{
				*res_ptr = 255;
			}
			else
			{
				unsigned char ndepth = static_cast<unsigned char>(((1 / (0.5f + dot)) * 128));
				*res_ptr = 0;
			}
		}
	}
}


int oclRSWorldProcessor::initMem(ocl_container * container, int width, int height)
{
	cl_int err = CL_SUCCESS;

	inputSize = getAllignedBufferSize(width, height, sizeof(cl_uint16));
	input = (cl_uint16*)allocAlligned(inputSize);

	outputSize = getAllignedBufferSize(width, height, sizeof(float) * 4);
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

	outputMem = clCreateBuffer(container->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, outputSize, output, &err);
	if (CL_SUCCESS != err)
	{
		LogError("Error: clCreateImage for srcA returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	clmeminit = true;

	return err;

}

char * oclRSWorldProcessor::getSourceCode(int id)
{
	nuiFrameworkManager* ptr = static_cast<nuiFrameworkManager*>(getFrameworkPtr());
	char* source;
	size_t size;
	if (id == 0)
	{
		std::string str = ptr->getRelativeToStartupPath("modules\\ocl\\processWorld.cl");
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

int oclRSWorldProcessor::getFunctionsCount()
{
	return 1;
}
