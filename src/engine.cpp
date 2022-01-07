// Cracklings, 07/01/2022.

#include "engine.hpp"
#include <gsl/gsl>

ec::engine::engine(const std::string &app_name) {
  { // Instance
    vk::ApplicationInfo _vk_application_info;
    _vk_application_info.apiVersion = VK_API_VERSION_1_2;
    _vk_application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    _vk_application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    _vk_application_info.pEngineName = "Engine Chromatique";
    _vk_application_info.pApplicationName = app_name.c_str();

    vk::InstanceCreateInfo _vk_instance_create_info;
    _vk_instance_create_info.enabledExtensionCount = _instance_extensions.size();
    _vk_instance_create_info.ppEnabledExtensionNames = _instance_extensions.data();
    _vk_instance_create_info.enabledLayerCount = _instance_layers.size();
    _vk_instance_create_info.ppEnabledLayerNames = _instance_layers.data();
    _vk_instance_create_info.pApplicationInfo = &_vk_application_info;

    _vk_instance = vk::createInstance(_vk_instance_create_info);
    Ensures(_vk_instance);
  }

  { // Physical Device
    auto _vk_physical_devices = _vk_instance.enumeratePhysicalDevices();
    for (auto _pd: _vk_physical_devices) {
      auto _pd_props = _pd.getProperties();
      if (_pd_props.vendorID == 0x10DE) {
        _vk_physical_device = _pd;
      }
      break;
    }
    Ensures(_vk_physical_device);
  }

  { // Device
    auto _qf_props = _vk_physical_device.getQueueFamilyProperties();
    for (int _qf_index = 0; _qf_index < _qf_props.size(); ++_qf_index) {
      if (_qf_props[_qf_index].queueFlags & vk::QueueFlagBits::eGraphics) {
        _vk_graphic_queue_family_index = _qf_index;
      }
    }

    vk::DeviceCreateInfo _vk_device_create_info;
    // TODO
    _vk_physical_device.createDevice(_vk_device_create_info);
  }
}
