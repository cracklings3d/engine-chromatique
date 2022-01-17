// Crack, 17/01/2022

#ifndef EC_SWAPCHAIN_HPP
#define EC_SWAPCHAIN_HPP

#include "surface.hpp"

#include <vulkan/vulkan.hpp>

namespace ec {
struct swapchain {
  vk::SwapchainKHR _vk_swapchain;

  std::vector<vk::Image> _vk_images;
  std::vector<vk::ImageView> _vk_image_views;

  std::shared_ptr<surface> _surface;
};
} // namespace ec

#endif //EC_SWAPCHAIN_HPP
