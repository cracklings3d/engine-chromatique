// Cracklings3D, 27/01/2022

#ifndef EC_SWAPCHAIN_HPP
#define EC_SWAPCHAIN_HPP

#include "surface.hpp"

#include <vulkan/vulkan.hpp>

namespace ec {
struct swapchain {
  vk::SwapchainKHR _vk_swapchain;

  vk::Format _vk_image_format;
  vk::ColorSpaceKHR _vk_color_space;

  std::vector<vk::Image> _vk_images;
  std::vector<vk::ImageView> _vk_image_views;

  std::shared_ptr<surface> _surface;
};
} // namespace ec

#endif //EC_SWAPCHAIN_HPP
