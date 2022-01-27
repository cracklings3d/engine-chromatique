// Cracklings, 07/01/2022.

#ifndef EC_ENGINE_HPP
#define EC_ENGINE_HPP

#include "vulkan_rhi/surface.hpp"
#include "vulkan_rhi/swapchain.hpp"
#include "vulkan_rhi/command_pool.hpp"
#include "vulkan_rhi/command_pool.hpp"
#include "vulkan_rhi/device.hpp"

#include "GLFW/glfw3.h"

#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <string>
#include <vulkan/vulkan.hpp>

namespace ec {
class app;
struct surface;

class engine {
public:
  explicit engine(const std::string &app_name = "");
  ~engine();

  void init(const std::shared_ptr<ec::surface> &surface);
  void update_scene();

  [[nodiscard]] vk::Instance get_instance() const;

private:
  const std::vector<const char *> _device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  std::shared_ptr<swapchain> _swapchain = std::make_shared<swapchain>();
  std::shared_ptr<surface> _surface = std::make_shared<surface>();
  std::shared_ptr<device> _device = nullptr;
  std::shared_ptr<command_pool> _command_pool = nullptr;
  std::shared_ptr<command_buffer> _command_buffer = nullptr;

  vk::Instance _vk_instance;

  // TODO: Depth buffer
  vk::Image _vk_image_depth_buffer;
  vk::DeviceMemory _vk_memory_depth_buffer;
  vk::ImageView _vk_image_view_depth_buffer;

  vk::Buffer _vk_buffer_uniform_buffer;
  vk::DeviceMemory _vk_memory_uniform_buffer;

  std::vector<vk::DescriptorPoolSize> _vk_descriptor_pool_size;
  std::vector<vk::DescriptorSetLayout> _vk_descriptor_set_layout_list_uniform_buffer;
  vk::DescriptorPool _vk_descriptor_pool;
  std::vector<vk::DescriptorSet> _vk_descriptor_set_list_uniform_buffer;

  vk::PipelineLayout _vk_pipeline_layout;

  vk::RenderPass _vk_render_pass;

  const std::vector<float> _vk_queue_priority_list = {1.0f};
  int _vk_queue_family_index = -1;

  [[nodiscard]] uint32_t get_memory_type_index(const vk::MemoryRequirements &requirement,
                                               const vk::MemoryPropertyFlags &flags) const;
};
} // namespace ec

#endif //EC_ENGINE_HPP
