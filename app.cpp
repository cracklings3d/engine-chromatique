#include <glfw/glfw3.h>

int main() {
  glfwInit();
  auto w = glfwCreateWindow(2560, 1440, "App Chromatique", nullptr, nullptr);
  glfwShowWindow(w);
  while (!glfwWindowShouldClose(w)) {
    glfwPollEvents();
  }
  glfwDestroyWindow(w);
  glfwTerminate();
  return 0;
}
