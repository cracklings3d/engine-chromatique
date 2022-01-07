// Cracklings, 07/01/2022.

#ifndef EC_ENGINE_HPP
#define EC_ENGINE_HPP

#include <string>
#include <vulkan/vulkan.hpp>

namespace ec {
class engine {
public:
  engine(const std::string &app_name = "");

private:
  inline const static std::vector<const char *> _instance_layers{"VK_LAYER_KHRONOS_validation"};
  inline const static std::vector<const char *> _instance_extensions = {};

  vk::Instance _vk_instance;
};
} // namespace ec

#endif //EC_ENGINE_HPP
