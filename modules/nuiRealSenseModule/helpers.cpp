#include "helpers.h"


cl_float4 castQuaternionToFloat4(Eigen::Quaternion<float> quaternion)
{
	cl_float4 res;
	res.w = quaternion.w();
	res.x = quaternion.x();
	res.y = quaternion.y();
	res.z = quaternion.z();

	return res;
}
