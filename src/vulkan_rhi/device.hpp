// Cracklings3D, 27/01/2022

#ifndef EC_DEVICE_HPP
#define EC_DEVICE_HPP

#include <vulkan/vulkan.hpp>

namespace ec {
class device {
public:
  explicit device(const vk::Instance &instance);

  void init();

  [[nodiscard]] vk::PhysicalDevice get_vk_physical_device() const;
  [[nodiscard]] vk::Device get_vk_device() const;

private:
  const std::vector<const char *> _device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  vk::Instance _vk_instance;
  vk::PhysicalDevice _vk_physical_device;
  vk::Device _vk_device;
  vk::Queue _vk_queue;

  const std::vector<float> _vk_queue_priority_list = {1.0f};
  int _vk_queue_family_index = -1;

  friend class command_pool;
};
} // namespace ec

#endif //EC_DEVICE_HPP
