#include "rs2DeviceContainer.h"

const std::string platform_camera_name = "Platform Camera";

rs2DeviceContainer::rs2DeviceContainer()
{
}


rs2DeviceContainer::~rs2DeviceContainer()
{
}

void rs2DeviceContainer::initDevices(rs2::context& context)
{
	for (auto&& dev : context.query_devices()) // Query the list of connected RealSense devices
	{
		if (isRealsenseDevice(dev))
		{
			std::string serial_number(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
			devices.emplace(std::pair<std::string, rs2::device>(serial_number, dev));
		}
	}
}

void rs2DeviceContainer::updateDevices(rs2::event_information & info)
{
	_mutex.lock();
	for (std::map<std::string, rs2::device>::iterator it = devices.begin(); it != devices.end(); it++)
	{
		std::pair<std::string, rs2::device> dev = *it;
		if (info.was_removed(dev.second))
		{
			devices.erase(it);
			std::map<rs2::device, rs2::pipeline_profile>::iterator profIt = profiles.find(dev.second);
			if (profIt != profiles.end())
			{
				profiles.erase(profIt);
			}
		}
	}

	rs2::device_list addedDevices = info.get_new_devices();
	for (rs2::device_list::device_list_iterator it = addedDevices.begin(); it != addedDevices.end(); it.operator++())
	{
		rs2::device dev = *it;
		if (info.was_added(dev) && isRealsenseDevice(dev))
		{
			std::string serial_number(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
			devices.emplace(std::pair<std::string, rs2::device>(serial_number, dev));
		}

		
	}
	_mutex.unlock();
}

rs2::device rs2DeviceContainer::getDeviceAt(int index)
{
	size_t i = 0;
	for (std::map<std::string, rs2::device>::iterator it = devices.begin(); it != devices.end(); it++, i++)
	{
		if (i == index)
			return (*it).second;
	}

	throw _exception();
}

size_t rs2DeviceContainer::getDevicesCount()
{
	return devices.size();
}

rs2::pipeline rs2DeviceContainer::getPipeline(rs2::device & device)
{
	std::string serial_number(device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
	_mutex.lock();
	/*if (devices.find(serial_number) != devices.end())
	{
		throw _exception();
	}*/

	rs2::pipeline p;
	rs2::config c;
	c.enable_device(serial_number);
	rs2::pipeline_profile profile = p.start(c);
	profiles.emplace(std::pair<rs2::device&, rs2::pipeline_profile>(device, profile));

	_mutex.unlock();

	return p;
}

rs2::pipeline rs2DeviceContainer::getPipeline(int index)
{
	rs2::device dev = getDeviceAt(index);
	return getPipeline(dev);
}

rs2::pipeline_profile rs2DeviceContainer::getPipelineProfile(rs2::device& device)
{
	return (*profiles.find(device)).second;
}

bool rs2DeviceContainer::isRealsenseDevice(rs2::device device)
{
	return platform_camera_name != device.get_info(RS2_CAMERA_INFO_NAME);	
}
