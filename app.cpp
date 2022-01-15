#include <gsl/gsl>
#include "app.hpp"

ec::app::app(const std::string& app_name) {
  _app_name = app_name;
  Ensures(glfwInit());
  _window = glfwCreateWindow(2560, 1440, app_name.c_str(), nullptr, nullptr);
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
  _engine->set_vk_surface(vk_surface);
  _engine->init();
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
