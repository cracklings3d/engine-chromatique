// Cracklings, 07/01/2022.

#ifndef EC_ENGINE_HPP
#define EC_ENGINE_HPP

#include <string>
#include <vulkan/vulkan.hpp>

namespace ec {
class engine {
public:
  explicit engine(const std::string &app_name = "");

private:
  const std::vector<const char *> _instance_layers{"VK_LAYER_KHRONOS_validation"};
  const std::vector<const char *> _instance_extensions = {};

  vk::Instance _vk_instance;
  vk::PhysicalDevice _vk_physical_device;
  vk::Device _vk_device;

  int _vk_graphic_queue_family_index;
};
} // namespace ec

#endif //EC_ENGINE_HPP
