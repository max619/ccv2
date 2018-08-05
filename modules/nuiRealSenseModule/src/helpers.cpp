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

Eigen::Vector3f getWorldCoordAt(rs2::depth_frame& frame, int x, int y, rs2_intrinsics& intrinsics, float scale)
{
	Eigen::Vector3f res;
	uint16_t* ptr = ((uint16_t*)(frame.get_data()) + (intrinsics.width * y + x) * sizeof(unsigned char));

	float ddepth = *ptr * scale;

	res.x() = ((x - intrinsics.ppx) / intrinsics.fx) * ddepth;
	res.y() = ((y - intrinsics.ppy) / intrinsics.fy) * ddepth;
	res.z() = ddepth;

	return res;
}
