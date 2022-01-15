// Cracklings, 07/01/2022.

#ifndef EC_ENGINE_HPP
#define EC_ENGINE_HPP

#include <glfw/glfw3.h>

#include <string>
#include <vulkan/vulkan.hpp>

namespace ec {
class app;

class engine {
public:
  explicit engine(const std::string &app_name = "");
  void init();

  [[nodiscard]] vk::Instance get_instance() const;

  void set_vk_surface(const VkSurfaceKHR &surface);

private:
  const std::vector<const char *> _device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  vk::SurfaceKHR _vk_surface;
  vk::SwapchainKHR _vk_swapchain;

  vk::Instance _vk_instance;
  vk::PhysicalDevice _vk_physical_device;
  vk::Device _vk_device;
  vk::Queue _vk_queue;
  vk::CommandPool _vk_command_pool;
  std::vector<vk::CommandBuffer> _vk_command_buffer;

  const std::vector<float_t> _vk_queue_priority_list = {1.0f};
  int _vk_queue_family_index = -1;
};
} // namespace ec

#endif //EC_ENGINE_HPP
