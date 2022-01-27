// Cracklings3D, 27/01/2022

#include "command_pool.hpp"
#include <gsl/gsl>

ec::command_buffer::command_buffer(const std::shared_ptr<device> &device,
                                   const std::shared_ptr<command_pool> &command_pool) {
  vk::CommandBufferAllocateInfo _vkai_command_buffer;
  _vkai_command_buffer.commandPool = command_pool->_vk_command_pool;
  _vkai_command_buffer.commandBufferCount = 1;
  _vkai_command_buffer.level = vk::CommandBufferLevel::ePrimary;
  _vk_command_buffer_list = device->get_vk_device().allocateCommandBuffers(_vkai_command_buffer);
  Ensures(!_vk_command_buffer_list.empty());
}

ec::command_pool::command_pool(const std::shared_ptr<device> &device) {
  vk::CommandPoolCreateInfo _vkci_command_pool;
  _vkci_command_pool.queueFamilyIndex = device->_vk_queue_family_index;
  _vk_command_pool = device->_vk_device.createCommandPool(_vkci_command_pool);
  Ensures(_vk_command_pool);
}
