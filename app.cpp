#include "app.hpp"

ec::app::app(const std::string& app_name) {
  _app_name = app_name;
  glfwInit();
  _window = glfwCreateWindow(2560, 1440, app_name.c_str(), nullptr, nullptr);
}

ec::app::~app() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void ec::app::init() {
  _engine = std::make_shared<engine>(_app_name);
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
