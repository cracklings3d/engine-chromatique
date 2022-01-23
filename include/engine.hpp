// Cracklings, 07/01/2022.

#ifndef EC_ENGINE_HPP
#define EC_ENGINE_HPP

#include "surface.hpp"
#include "swapchain.hpp"

#include <GLFW/glfw3.h>

#include <string>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace ec {
class app;
struct surface;

class engine {
public:
  explicit engine(const std::string &app_name = "");
  ~engine();

  void init(std::shared_ptr<ec::surface> surface);

  [[nodiscard]] vk::Instance get_instance() const;

private:
  const std::vector<const char *> _device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  std::shared_ptr<swapchain> _swapchain = std::make_shared<swapchain>();
  std::shared_ptr<surface> _surface = std::make_shared<surface>();

  vk::Instance _vk_instance;
  vk::PhysicalDevice _vk_physical_device;
  vk::Device _vk_device;
  vk::Queue _vk_queue;
  vk::CommandPool _vk_command_pool;
  std::vector<vk::CommandBuffer> _vk_command_buffer;

  // TODO: Depth buffer
  vk::Image _vk_image_depth_buffer;
  vk::DeviceMemory _vk_memory_depth_buffer;
  vk::ImageView _vk_image_view_depth_buffer;

  vk::Buffer _vk_buffer_uniform_buffer;
  vk::DeviceMemory _vk_memory_uniform_buffer;

  const std::vector<float> _vk_queue_priority_list = {1.0f};
  int _vk_queue_family_index = -1;

  [[nodiscard]] uint32_t get_memory_type_index(const vk::MemoryRequirements &requirement) const;
};
} // namespace ec

#endif //EC_ENGINE_HPP
