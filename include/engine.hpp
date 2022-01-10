// Cracklings, 07/01/2022.

#ifndef EC_ENGINE_HPP
#define EC_ENGINE_HPP

#include <glfw/glfw3.h>

#include <string>
#include <vulkan/vulkan.hpp>

namespace ec {
class engine {
public:
  explicit engine(const std::string &app_name = "");

private:
  const std::vector<const char *> _instance_layers = {"VK_LAYER_KHRONOS_validation"};
//  std::vector<const char *> _instance_extensions = {};
  const std::vector<const char *> _device_extensions = {"VK_KHR_swapchain"};

  vk::Instance _vk_instance;
  vk::PhysicalDevice _vk_physical_device;
  vk::Device _vk_device;
  vk::Queue _vk_queue;

  const std::vector<float_t> _vk_queue_priority_list = {1.0f};
  int _vk_queue_family_index = -1;
};
} // namespace ec

#endif //EC_ENGINE_HPP
