#include "app.hpp"
#include <gsl/gsl>
#include <iostream>

ec::app::app(const std::string &app_name, uint32_t width, uint32_t height) {
  _app_name = app_name;

  glfwSetErrorCallback([](int err_code, const char *err_desc) {
    std::cout << err_desc << std::endl;
  });
  Ensures(glfwInit());
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  Ensures(glfwVulkanSupported());
  _window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), app_name.c_str(), nullptr, nullptr);
  int w, h;
  glfwGetWindowSize(_window, &w, &h);
//  glfwGetWindowAttrib(_window, )
  _surface._vk_surface_extent = vk::Extent2D{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
  Ensures(_window);
}

ec::app::~app() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void ec::app::init() {
  _engine = std::make_shared<engine>(_app_name);
  auto instance = _engine->get_instance();
  VkSurfaceKHR vk_surface;
  glfwCreateWindowSurface(instance, _window, nullptr, &vk_surface);
  _surface._vk_surface = vk_surface;
  _engine->init(_surface);
}

void ec::app::show() {
  glfwShowWindow(_window);
  while (!glfwWindowShouldClose(_window)) {
    glfwPollEvents();
  }
}

int main() {
  ec::app app{"App Chromatique"};
  app.init();
  app.show();
  return 0;
}
