// Cracklings, 07/01/2022.

#include "engine.hpp"
#include "surface.hpp"
#include <gsl/gsl>
#include <utility>

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
    Ensures(!_vk_command_buffer.empty());
  }
}

ec::engine::~engine() {
  // TODO
  //  _vk_device.destroy();
  //  _vk_instance.destroy();
}

void ec::engine::init(std::shared_ptr<ec::surface> surface) {
  _surface = std::move(surface);
  { // Swapchain
    auto supported_image_formats = _vk_physical_device.getSurfaceFormatsKHR(_surface->_vk_surface);
    _swapchain->_vk_image_format = supported_image_formats[0].format;
    _swapchain->_vk_color_space = supported_image_formats[0].colorSpace;
    // TODO: Pick format from supported ones.

    vk::SwapchainCreateInfoKHR _vkci_swapchain;
    _vkci_swapchain.minImageCount = 3;
    _vkci_swapchain.surface = _surface->_vk_surface;
    _vkci_swapchain.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    _vkci_swapchain.imageFormat = _swapchain->_vk_image_format;
    _vkci_swapchain.imageColorSpace = _swapchain->_vk_color_space;
    _vkci_swapchain.imageArrayLayers = 1;
    _vkci_swapchain.imageExtent = _surface->_vk_surface_extent;
    _vkci_swapchain.imageSharingMode = vk::SharingMode::eExclusive;
    _swapchain->_vk_swapchain = _vk_device.createSwapchainKHR(_vkci_swapchain);
    Ensures(_swapchain->_vk_swapchain);
    _swapchain->_surface = _surface;
  }

  { // Swapchain image views
    _swapchain->_vk_images = _vk_device.getSwapchainImagesKHR(_swapchain->_vk_swapchain);

    for (auto &_vk_image: _swapchain->_vk_images) {
      vk::ImageViewCreateInfo _vkci_image_view;
      _vkci_image_view.image = _vk_image;
      _vkci_image_view.viewType = vk::ImageViewType::e2D;
      _vkci_image_view.components.r = vk::ComponentSwizzle::eR;
      _vkci_image_view.components.g = vk::ComponentSwizzle::eG;
      _vkci_image_view.components.b = vk::ComponentSwizzle::eB;
      _vkci_image_view.components.a = vk::ComponentSwizzle::eA;
      _vkci_image_view.subresourceRange.layerCount = 1;
      _vkci_image_view.subresourceRange.baseArrayLayer = 0;
      _vkci_image_view.subresourceRange.levelCount = 1;
      _vkci_image_view.subresourceRange.baseMipLevel = 0;
      _vkci_image_view.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      _vkci_image_view.format = _swapchain->_vk_image_format;

      _vk_device.createImageView(_vkci_image_view);
      auto _vk_image_view = _vk_device.createImageView(_vkci_image_view);
      Ensures(_vk_image_view);

      _swapchain->_vk_image_views.push_back(_vk_image_view);
    }
    Ensures(_swapchain->_vk_image_views.size() == _swapchain->_vk_images.size());
  }

  { // Depth buffer
    vk::ImageCreateInfo _vkci_depth_image;
    _vkci_depth_image.format = vk::Format::eD16Unorm;
    _vkci_depth_image.extent.width = _surface->_vk_surface_extent.width;
    _vkci_depth_image.extent.height = _surface->_vk_surface_extent.height;
    _vkci_depth_image.extent.depth = 1;
    _vkci_depth_image.arrayLayers = 1;
    _vkci_depth_image.mipLevels = 1;
    _vkci_depth_image.imageType = vk::ImageType::e2D;
    _vkci_depth_image.sharingMode = vk::SharingMode::eExclusive;
    _vkci_depth_image.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    _vkci_depth_image.initialLayout = vk::ImageLayout::eUndefined;
    _vkci_depth_image.samples = vk::SampleCountFlagBits::e1;

    _vk_depth_image = _vk_device.createImage(_vkci_depth_image);
    Ensures(_vk_depth_image);

    auto _depth_memory_requirements = _vk_device.getImageMemoryRequirements(_vk_depth_image);
    vk::MemoryAllocateInfo _vkai_depth_buffer;
    _vkai_depth_buffer.allocationSize = _depth_memory_requirements.size;
    _vkai_depth_buffer.memoryTypeIndex = get_memory_type_index(_depth_memory_requirements);
    _vk_depth_buffer = _vk_device.allocateMemory(_vkai_depth_buffer);
    Ensures(_vk_depth_image);

    _vk_device.bindImageMemory(_vk_depth_image, _vk_depth_buffer, 0);

    vk::ImageViewCreateInfo _vkci_depth_view;
    _vkci_depth_view.image = _vk_depth_image;
    _vkci_depth_view.format = vk::Format::eD16Unorm;
    _vkci_depth_view.components.r = vk::ComponentSwizzle::eR;
    _vkci_depth_view.components.g = vk::ComponentSwizzle::eG;
    _vkci_depth_view.components.b = vk::ComponentSwizzle::eB;
    _vkci_depth_view.components.a = vk::ComponentSwizzle::eA;
    _vkci_depth_view.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
    _vkci_depth_view.subresourceRange.baseMipLevel = 0;
    _vkci_depth_view.subresourceRange.levelCount = 1;
    _vkci_depth_view.subresourceRange.baseArrayLayer = 0;
    _vkci_depth_view.subresourceRange.layerCount = 1;
    _vkci_depth_view.viewType = vk::ImageViewType::e2D;
    _vk_depth_view = _vk_device.createImageView(_vkci_depth_view);
    Ensures(_vk_depth_view);
  }

  { // Uniform buffer
  }
}

vk::Instance ec::engine::get_instance() const {
  return _vk_instance;
}
uint32_t ec::engine::get_memory_type_index(const vk::MemoryRequirements &requirement) const {
  Ensures(_vk_physical_device);
  auto props = _vk_physical_device.getMemoryProperties();
  for (int j = 0; j < props.memoryTypeCount; ++j) {
    auto type = props.memoryTypes[j];
    // We can do better than this
    if (type.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) return j;
  }
  return 0;
}
