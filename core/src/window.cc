#include <axolotl/window.h>

#include <axolotl/renderer.h>
#include <axolotl/gui.h>

#include <GLFW/glfw3.h>

namespace axl {

  Window::Window(u32 width, u32 height, const std::string &title):
    _window_height(height),
    _window_width(width),
    _window_title(title),
    _window(nullptr),
    _renderer(nullptr)
  {
    glfwInit();
    // This is a renderer used for in-house testing, so we'll use latest OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    log::debug("widht: {}, height: {}, title: {}", width, height, title);
    _window = glfwCreateWindow(_window_width, _window_height, _window_title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(_window, this);

    _renderer = new Renderer(this);
    _gui = new GUI(_renderer, this);
  }

  Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();

    delete _gui;
    delete _renderer;
  }

  bool Window::Update() {
    glfwPollEvents();

    f64 time_now = glfwGetTime();
    _delta_time = time_now - _time_last;
    _time_last = time_now;

    _gui->Update();
    return !glfwWindowShouldClose(_window);
  }

  void Window::Draw() {
    _gui->Draw();
    glfwSwapBuffers(_window);
  }

  Renderer * Window::GetRenderer() const {
    return _renderer;
  }

  GLFWwindow * Window::GetGLFWWindow() const {
    return _window;
  }

  void Window::SetTitle(const std::string &title) {
    _window_title = title;
    glfwSetWindowTitle(_window, _window_title.c_str());
  }

  void Window::SetSize(u32 width, u32 height) {
    glfwSetWindowSize(_window, width, height);
    _window_width = width;
    _window_height = height;
    _renderer->Resize(width, height);
  }

  f64 Window::GetDeltaTime() const {
    return _delta_time * 1000;
  }

} // namespace axolotl
