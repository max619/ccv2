/**
* \file      calcWorldCoordinatesNormal.cl
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

constant sampler_t smplr = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void calcWorldCoordinatesNormal(float ppx, float ppy, float fx, float fy, float depthmul, read_only image2d_t imageA, write_only image2d_t imageC)
{
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	float u = (x - ppx) / fx;
	float v = (y - ppy) / fy;

	float ddepth = (read_imageui(imageA, smplr, (int2)(x, y)).x) * depthmul;

	float4 pix = (float4)(ddepth * u, ddepth * v, ddepth, 0.0f);	

	write_imagef(imageC, (int2)(x, y), pix);
}