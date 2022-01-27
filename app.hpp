// Cracklings, 07/01/2022.

#ifndef EC_APP_H
#define EC_APP_H

#include <memory>
#include <string>

#include "src/vulkan_rhi/surface.hpp"
#include <vulkan/vulkan.hpp>
// clang-format sorting barrier
#include <GLFW/glfw3.h>

#include "src/engine.hpp"

namespace ec {

class app {
public:
  explicit app(const std::string &app_name, uint32_t width = 1920, uint32_t height = 1080);
  ~app();

  void init();
  void show();

private:
  std::string _app_name;
  GLFWwindow *_window;
  surface _surface;
  std::shared_ptr<engine> _engine;
};
} // namespace ec

#endif //EC_APP_H
