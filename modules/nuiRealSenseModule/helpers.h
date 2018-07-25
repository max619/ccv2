#ifndef HELPERS
#define HELPERS

#include <Eigen/Geometry>
#include <CL/cl.hpp>

cl_float4 castQuaternionToFloat4(Eigen::Quaternion<float> quaternion);



#endif