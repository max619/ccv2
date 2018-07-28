#ifndef HELPERS
#define HELPERS
#define EIGEN_DONT_ALIGN_STATICALLY

#include <Eigen/Geometry>
#include <CL/cl.hpp>
#include <librealsense2/rs.hpp>

cl_float4 castQuaternionToFloat4(Eigen::Quaternion<float> quaternion);

Eigen::Vector3f getWorldCoordAt(rs2::depth_frame& frame, int x, int y, rs2_intrinsics& intrinsics, float scale);


#endif