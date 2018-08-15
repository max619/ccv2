/**
* \file      rotate.cl
* \author    Maxim Bagryantsev
* \date      2018
* \copyright Copyright 2012 NUI Group. All rights reserved.
* \copyright Copyright 2018 Appollo Pro. All rights reserved.
*/

constant sampler_t smplr = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void rotateWorld(float4 quat, read_only image2d_t imageA, write_only image2d_t imageC)
{
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	float4 coord = (read_imagef(imageA, smplr, (int2)(x, y)).x);

	float4 fmul;

	fmul.w = coord.w * quat.w - coord.x * quat.x - coord.y * quat.y - coord.z * quat.z;
	fmul.x = coord.w * quat.x + coord.x * quat.w + coord.y * quat.z + coord.z * quat.y;
	fmul.y = coord.w * quat.y + coord.y * quat.w + coord.z * quat.x + coord.x * quat.z;
	fmul.z = coord.w * quat.z + coord.z * quat.w + coord.x * quat.y + coord.y * quat.x;

	float4 res;

	res.w = fmul.w * quat.w + fmul.x * quat.x + fmul.y * quat.y + fmul.z * quat.z;
	res.x = - fmul.w * quat.x + fmul.x * quat.w - fmul.y * quat.z - fmul.z * quat.y;
	res.y = - fmul.w * quat.y + fmul.y * quat.w - fmul.z * quat.x - fmul.x * quat.z;
	res.z = - fmul.w * quat.z + fmul.z * quat.w - fmul.x * quat.y - fmul.y * quat.x;
	
	write_imagef(imageC, (int2)(x, y), res);
}