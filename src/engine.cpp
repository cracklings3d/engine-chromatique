// Cracklings, 07/01/2022.

#include "engine.hpp"
#include "vulkan_rhi/surface.hpp"
#include <gsl/gsl>
#include <memory>
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

  _device = std::make_shared<device>(_vk_instance);
  _device->init();

  _command_pool = std::make_shared<command_pool>(_device);
  _command_buffer = std::make_shared<command_buffer>(_device, _command_pool);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-equals-default"
ec::engine::~engine() {
  // TODO
  //  _device->get_vk_device().destroy();
  //  _vk_instance.destroy();
}
#pragma clang diagnostic pop

void ec::engine::init(const std::shared_ptr<ec::surface> &surface) {
  _surface = surface;

  { // Swapchain
    auto supported_image_formats = _device->get_vk_physical_device().getSurfaceFormatsKHR(_surface->_vk_surface);
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
    _swapchain->_vk_swapchain = _device->get_vk_device().createSwapchainKHR(_vkci_swapchain);
    Ensures(_swapchain->_vk_swapchain);
    _swapchain->_surface = _surface;
  }

  { // Swapchain image views
    _swapchain->_vk_images = _device->get_vk_device().getSwapchainImagesKHR(_swapchain->_vk_swapchain);

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

      _device->get_vk_device().createImageView(_vkci_image_view);
      auto _vk_image_view = _device->get_vk_device().createImageView(_vkci_image_view);
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

    _vk_image_depth_buffer = _device->get_vk_device().createImage(_vkci_depth_image);
    Ensures(_vk_image_depth_buffer);

    auto _memory_requirements = _device->get_vk_device().getImageMemoryRequirements(_vk_image_depth_buffer);
    vk::MemoryAllocateInfo _vkai_depth_buffer;
    _vkai_depth_buffer.allocationSize = _memory_requirements.size;
    _vkai_depth_buffer.memoryTypeIndex =
        get_memory_type_index(_memory_requirements, vk::MemoryPropertyFlagBits::eDeviceLocal);
    _vk_memory_depth_buffer = _device->get_vk_device().allocateMemory(_vkai_depth_buffer);
    Ensures(_vk_image_depth_buffer);

    _device->get_vk_device().bindImageMemory(_vk_image_depth_buffer, _vk_memory_depth_buffer, 0);

    vk::ImageViewCreateInfo _vkci_depth_view;
    _vkci_depth_view.image = _vk_image_depth_buffer;
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
    _vk_image_view_depth_buffer = _device->get_vk_device().createImageView(_vkci_depth_view);
    Ensures(_vk_image_view_depth_buffer);
  }

  { // Uniform buffer
    // clang-format off
    glm::vec3 eye     {0, 0, 30};
    glm::vec3 world_up{0, 1, 0};
    glm::vec3 origin  {};
    glm::mat view = glm::lookAt(eye, origin, world_up);
    glm::mat proj = glm::perspective(3.14159f / 2, 16 / 9.0f, 0.01f, 10000.0f);
    glm::mat clip = glm::mat4(1.0f,  0.0f, 0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f, 0.0f,
                              0.0f,  0.0f, 0.5f, 0.0f,
                              0.0f,  0.0f, 0.5f, 1.0f);

    glm::mat mvp = clip * proj * view;
    // clang-format on

    vk::BufferCreateInfo _vkci_uniform_buffer;
    _vkci_uniform_buffer.size = sizeof(mvp);
    _vkci_uniform_buffer.usage = vk::BufferUsageFlagBits::eUniformBuffer;
    _vkci_uniform_buffer.queueFamilyIndexCount = 0;
    _vkci_uniform_buffer.pQueueFamilyIndices = nullptr;
    _vkci_uniform_buffer.sharingMode = vk::SharingMode::eExclusive;
    _vk_buffer_uniform_buffer = _device->get_vk_device().createBuffer(_vkci_uniform_buffer);
    Ensures(_vk_buffer_uniform_buffer);

    auto _memory_requirements = _device->get_vk_device().getBufferMemoryRequirements(_vk_buffer_uniform_buffer);
    vk::MemoryAllocateInfo _vkai_uniform_buffer;
    _vkai_uniform_buffer.allocationSize = _memory_requirements.size;
    _vkai_uniform_buffer.memoryTypeIndex =
        get_memory_type_index(_memory_requirements, vk::MemoryPropertyFlagBits::eHostVisible);
    _vk_memory_uniform_buffer = _device->get_vk_device().allocateMemory(_vkai_uniform_buffer);
    Ensures(_vk_memory_uniform_buffer);

    auto _uniform_buffer_handle =
        _device->get_vk_device().mapMemory(_vk_memory_uniform_buffer, 0, _memory_requirements.size);
    memcpy(_uniform_buffer_handle, &mvp, sizeof(mvp));
    _device->get_vk_device().unmapMemory(_vk_memory_uniform_buffer);

    _device->get_vk_device().bindBufferMemory(_vk_buffer_uniform_buffer, _vk_memory_uniform_buffer, 0);
  }

  { // Descriptor set uniform buffer
   {// Layout
    vk::DescriptorSetLayoutBinding _vk_descriptor_set_layout_binding;
  _vk_descriptor_set_layout_binding.binding = 0;
  _vk_descriptor_set_layout_binding.descriptorType = vk::DescriptorType::eUniformBuffer;
  _vk_descriptor_set_layout_binding.descriptorCount = 1;
  _vk_descriptor_set_layout_binding.stageFlags = vk::ShaderStageFlagBits::eVertex;

  vk::DescriptorSetLayoutCreateInfo _vkci_descriptor_set_layout;
  _vkci_descriptor_set_layout.bindingCount = 1;
  _vkci_descriptor_set_layout.pBindings = &_vk_descriptor_set_layout_binding;

  auto layout0 = _device->get_vk_device().createDescriptorSetLayout(_vkci_descriptor_set_layout);
  _vk_descriptor_set_layout_list_uniform_buffer.push_back(layout0);
  Ensures(_vk_descriptor_set_layout_list_uniform_buffer.size() == 1);
}
{ // Resource
  vk::DescriptorPoolSize size0 = {};
  size0.descriptorCount = 1;
  size0.type = vk::DescriptorType::eUniformBuffer;
  _vk_descriptor_pool_size.push_back(size0);

  vk::DescriptorPoolCreateInfo _vkci_descriptor_pool;
  _vkci_descriptor_pool.maxSets = 1;
  _vkci_descriptor_pool.pPoolSizes = _vk_descriptor_pool_size.data();
  _vkci_descriptor_pool.poolSizeCount = _vk_descriptor_pool_size.size();
  _vk_descriptor_pool = _device->get_vk_device().createDescriptorPool(_vkci_descriptor_pool);
  Ensures(_vk_descriptor_pool);

  vk::DescriptorSetAllocateInfo _vkai_descriptor_set;
  _vkai_descriptor_set.descriptorPool = _vk_descriptor_pool;
  _vkai_descriptor_set.descriptorSetCount = _vk_descriptor_set_layout_list_uniform_buffer.size();
  _vkai_descriptor_set.pSetLayouts = _vk_descriptor_set_layout_list_uniform_buffer.data();
  _vk_descriptor_set_list_uniform_buffer = _device->get_vk_device().allocateDescriptorSets(_vkai_descriptor_set);
  Ensures(_vk_descriptor_set_list_uniform_buffer.size() == 1);
}

{ // Pipeline
  vk::PipelineLayoutCreateInfo _vkci_pipeline_layout;
  _vkci_pipeline_layout.setLayoutCount = _vk_descriptor_set_layout_list_uniform_buffer.size();
  _vkci_pipeline_layout.pSetLayouts = _vk_descriptor_set_layout_list_uniform_buffer.data();
  _vk_pipeline_layout = _device->get_vk_device().createPipelineLayout(_vkci_pipeline_layout);
  Ensures(_vk_pipeline_layout);
}
}

{ // Render pass
  {
      //

  }

  { // Pass assembling
    vk::RenderPassCreateInfo _vkci_render_pass;
    // TODO
    _vk_render_pass = _device->get_vk_device().createRenderPass(_vkci_render_pass);
    Ensures(_vk_render_pass);
  }
}
}

vk::Instance ec::engine::get_instance() const { return _vk_instance; }

uint32_t ec::engine::get_memory_type_index(const vk::MemoryRequirements &requirement,
                                           const vk::MemoryPropertyFlags &flags) const {
  Ensures(_device->get_vk_physical_device());
  auto props = _device->get_vk_physical_device().getMemoryProperties();
  for (int j = 0; j < props.memoryTypeCount; ++j) {
    auto type = props.memoryTypes[j];
    // We can do better than this
    if (type.propertyFlags & flags) return j;
  }
  return 0;
}

void ec::engine::update_scene() {
  vk::DescriptorBufferInfo db;
  db.buffer = _vk_buffer_uniform_buffer;
  vk::WriteDescriptorSet _vk_write_descriptor_set;
  // TODO: fill this
  std::vector<vk::WriteDescriptorSet> _vk_write_descriptor_set_list = {_vk_write_descriptor_set};
  _device->get_vk_device().updateDescriptorSets(_vk_write_descriptor_set_list, {});
}
