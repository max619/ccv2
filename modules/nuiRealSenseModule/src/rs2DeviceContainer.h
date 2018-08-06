/////////////////////////////////////////////////////////////////////////////
// Name:        modules/rs2DeviceContainer.h
// Purpose:     Example Module to be used for developments.
// Author:      Maxim Bagryantsev
// Copyright:   (c) 2018 Appollo Pro
/////////////////////////////////////////////////////////////////////////////

#ifndef RS2_DEVICE_CONTAINER
#define RS2_DEVICE_CONTAINER
#include <librealsense2/rs.hpp>
#include <mutex>
#include <map>

class rs2DeviceContainer
{
public:
	rs2DeviceContainer();
	~rs2DeviceContainer();

	void initDevices(rs2::context& context);
	void updateDevices(rs2::event_information& info);
	rs2::device getDeviceAt(int index);
	size_t getDevicesCount();
	rs2::pipeline getPipeline(rs2::device& device);
	rs2::pipeline getPipeline(rs2::device & device, int width, int height, int fps);
	rs2::pipeline getPipeline(int index, int width, int height, int fps);
	rs2::pipeline getPipeline(int index);
	rs2::pipeline_profile getPipelineProfile(rs2::device& pipeline);


private:
	std::map<std::string, rs2::device> devices;
	std::map<rs2::device, rs2::pipeline_profile> profiles;
	std::mutex _mutex;
	bool isRealsenseDevice(rs2::device device);

};
#endif

