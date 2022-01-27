// Cracklings3D, 27/01/2022

#include <gsl/gsl>

#include "device.hpp"

void ec::device::init() {
  { // Physical Device
    auto _vk_physical_devices = _vk_instance.enumeratePhysicalDevices();
    for (auto _pd: _vk_physical_devices) {
      auto _pd_props = _pd.getProperties();
      //      printf("Vendor ID: %x\n", _pd_props.vendorID);
      if (_pd_props.vendorID == 0x10DE) { // NVidia
        _vk_physical_device = _pd;
      }
      break;
    }
    Ensures(_vk_physical_device);
  }

  { // Device
    auto _qf_props = _vk_physical_device.getQueueFamilyProperties();
    for (int _qf_index = 0; _qf_index < _qf_props.size(); ++_qf_index) {
      if (_qf_props[_qf_index].queueFlags & vk::QueueFlagBits::eGraphics) { _vk_queue_family_index = _qf_index; }
      break;
    }
    Ensures(_vk_queue_family_index >= 0);

    vk::DeviceQueueCreateInfo _vkci_queue;
    _vkci_queue.queueCount = 1;
    _vkci_queue.pQueuePriorities = _vk_queue_priority_list.data();
    _vkci_queue.queueFamilyIndex = _vk_queue_family_index;

    auto device_ext_props = _vk_physical_device.enumerateDeviceExtensionProperties();
    auto device_features = _vk_physical_device.getFeatures();

    vk::DeviceCreateInfo _vkci_device;
    _vkci_device.queueCreateInfoCount = 1;
    _vkci_device.pQueueCreateInfos = &_vkci_queue;
    _vkci_device.enabledExtensionCount = _device_extensions.size();
    _vkci_device.ppEnabledExtensionNames = _device_extensions.data();
    // TODO: Add _vkci_device.pEnabledFeatures

    _vk_device = _vk_physical_device.createDevice(_vkci_device);
    Ensures(_vk_device);

    _vk_queue = _vk_device.getQueue(_vk_queue_family_index, 0);
    Ensures(_vk_queue);
  }
}

ec::device::device(const vk::Instance &instance) {
  Ensures(instance);
  _vk_instance = instance;
}

vk::PhysicalDevice ec::device::get_vk_physical_device() const { return _vk_physical_device; }
vk::Device ec::device::get_vk_device() const { return _vk_device; }
