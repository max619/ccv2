constant sampler_t smplr = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void getTouchedPoints(float16 args, read_only image2d_t imageA, write_only image2d_t imageC)
{
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	float u = (x - args.s2) / args.s0;
	float v = (y - args.s3) / args.s1;

	float ddepth = (read_imageui(imageA, smplr, (int2)(x, y)).x) * args.s4;

	float4 coord = (float4)(ddepth * u, ddepth * v, ddepth, 0.0f);

	float3 vec;
	vec.x = coord.x - args.s8;
	vec.y = coord.y - args.s9;
	vec.z = coord.z - args.sA;

	float dott = vec.x * args.s5 + vec.y * args.s6 + vec.z * args.s7;

	if(dott > args.sB && dott < args.sC)
	{
		write_imageui(imageC, (int2)(x, y), 255);
	}
	else
	{
		write_imageui(imageC, (int2)(x, y), 0);
	}
}