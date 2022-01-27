// Cracklings3D, 27/01/2022

#ifndef EC_COMMAND_POOL_HPP
#define EC_COMMAND_POOL_HPP

#include "device.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace ec {
class device;

// TODO: consider rename this, since this is a collection of buffers.
class command_buffer {
public:
  explicit command_buffer(const std::shared_ptr<device> &device, const std::shared_ptr<command_pool> &command_pool);

private:
  std::vector<vk::CommandBuffer> _vk_command_buffer_list;
};

class command_pool {
public:
  explicit command_pool(const std::shared_ptr<device> &device);

private:
  vk::Device _vk_device;
  vk::CommandPool _vk_command_pool;

  friend class command_buffer;
};

} // namespace ec

#endif //EC_COMMAND_POOL_HPP
