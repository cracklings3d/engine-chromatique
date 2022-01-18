// cracklings, 16/01/2022.

#ifndef ENGINE_CHROMATIQUE_SURFACE_HPP
#define ENGINE_CHROMATIQUE_SURFACE_HPP

#include <vulkan/vulkan.hpp>

namespace ec {
struct surface {
  vk::SurfaceKHR _vk_surface;
  vk::Extent2D _vk_surface_extent;

  // TODO: Format?
};
} // namespace ec

#endif //ENGINE_CHROMATIQUE_SURFACE_HPP
