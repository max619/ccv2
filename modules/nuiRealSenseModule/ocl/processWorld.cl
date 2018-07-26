constant sampler_t smplr = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void processWorld(float16 args, read_only image2d_t imageA, __global float4* imageC)
{
	const int x = get_global_id(0);
	const int y = get_global_id(1);

	float u = (x - args.s2) / args.s0;
	float v = (y - args.s3) / args.s1;

	int2 size = get_image_dim(imageA);
	int offset = y * size.x + x;

	float ddepth = (read_imageui(imageA, smplr, (int2)(x, y)).x) * args.s4;

	float4 coord = (float4)(0.0f, ddepth * u, ddepth * v, ddepth);

	float4 fmul;

	fmul.w = coord.w * args.s8 - coord.x * args.s5 - coord.y * args.s6 - coord.z * args.s7;
	fmul.x = coord.w * args.s5 + coord.x * args.s8 + coord.y * args.s7 + coord.z * args.s6;
	fmul.y = coord.w * args.s6 + coord.y * args.s8 + coord.z * args.s5 + coord.x * args.s7;
	fmul.z = coord.w * args.s7 + coord.z * args.s8 + coord.x * args.s6 + coord.y * args.s5;

	float4 res;

	res.w = fmul.w * args.s8 + fmul.x * args.s5 + fmul.y * args.s6 + fmul.z * args.s7;
	res.x = -fmul.w * args.s5 + fmul.x * args.s8 - fmul.y * args.s7 - fmul.z * args.s6;
	res.y = -fmul.w * args.s6 + fmul.y * args.s8 - fmul.z * args.s5 - fmul.x * args.s7;
	res.z = -fmul.w * args.s7 + fmul.z * args.s8 - fmul.x * args.s6 - fmul.y * args.s5;

	imageC[offset] = res;
}