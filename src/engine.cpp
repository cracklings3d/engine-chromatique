// Cracklings, 07/01/2022.

#include "engine.hpp"
#include "surface.hpp"
#include <gsl/gsl>

ec::engine::engine(const std::string &app_name) {
  { // Instance
    vk::ApplicationInfo _vk_application_info;
    _vk_application_info.apiVersion = VK_API_VERSION_1_2;
    _vk_application_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    _vk_application_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    _vk_application_info.pEngineName = "Engine Chromatique";
    _vk_application_info.pApplicationName = app_name.c_str();

    uint32_t required_device_extension_count;
    glfwGetRequiredInstanceExtensions(&required_device_extension_count);
    auto required_device_extension_list = glfwGetRequiredInstanceExtensions(&required_device_extension_count);

    vk::InstanceCreateInfo _vkci_instance;
    _vkci_instance.enabledExtensionCount = required_device_extension_count;
    _vkci_instance.ppEnabledExtensionNames = required_device_extension_list;
    //    _vkci_instance.enabledLayerCount = _instance_layers.size();
    //    _vkci_instance.ppEnabledLayerNames = _instance_layers.data();
    _vkci_instance.pApplicationInfo = &_vk_application_info;

    _vk_instance = vk::createInstance(_vkci_instance);
    Ensures(_vk_instance);
  }

  { // Physical Device
    auto _vk_physical_devices = _vk_instance.enumeratePhysicalDevices();
    for (auto _pd: _vk_physical_devices) {
      auto _pd_props = _pd.getProperties();
      printf("%x\n", _pd_props.vendorID);
      //      if (_pd_props.vendorID == 0x10DE) {
      _vk_physical_device = _pd;
      //      }
      break;
    }
    Ensures(_vk_physical_device);
  }

  { // Device
    auto _qf_props = _vk_physical_device.getQueueFamilyProperties();
    for (int _qf_index = 0; _qf_index < _qf_props.size(); ++_qf_index) {
      if (_qf_props[_qf_index].queueFlags & vk::QueueFlagBits::eGraphics) {
        _vk_queue_family_index = _qf_index;
      }
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

  { // Record commands
    // TODO: move out of ctor.
    vk::CommandPoolCreateInfo _vkci_command_pool;
    _vkci_command_pool.queueFamilyIndex = _vk_queue_family_index;
    _vk_command_pool = _vk_device.createCommandPool(_vkci_command_pool);
    Ensures(_vk_command_pool);

    vk::CommandBufferAllocateInfo _vkai_command_buffer;
    _vkai_command_buffer.commandPool = _vk_command_pool;
    _vkai_command_buffer.commandBufferCount = 1;
    _vkai_command_buffer.level = vk::CommandBufferLevel::ePrimary;
    _vk_command_buffer = _vk_device.allocateCommandBuffers(_vkai_command_buffer);
    Ensures(_vk_command_buffer.size() > 0);
  }
}

void ec::engine::init(ec::surface _surface) {
  { // Swapchain
    vk::SwapchainCreateInfoKHR _vkci_swapchain;
    _vkci_swapchain.minImageCount = 3;
    _vkci_swapchain.surface = _surface._vk_surface;
    _vkci_swapchain.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    _vkci_swapchain.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    _vkci_swapchain.imageArrayLayers = 1;
    _vkci_swapchain.imageExtent = _surface._vk_surface_extent;
    _vk_device.createSwapchainKHR(_vkci_swapchain);
  }
}

vk::Instance ec::engine::get_instance() const {
  return _vk_instance;
}
void ec::engine::set_vk_surface(VkSurfaceKHR const &surface) {
  _vk_surface = surface;
}
