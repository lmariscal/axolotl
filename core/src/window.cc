#include <GLFW/glfw3.h>
#include <axolotl/gui.hh>
#include <axolotl/renderer.hh>
#include <axolotl/window.hh>

namespace axl {

  Window::Window(u32 width, u32 height, const std::string &title):
    _window_height(height),
    _window_width(width),
    _window_title(title),
    _window(nullptr),
    _renderer(nullptr),
    _io_manager(new IOManager(*this)),
    _lock_mouse(false) {
    glfwInit();
    // This is a renderer used for in-house testing, so we'll use latest OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    _window = glfwCreateWindow(_window_width, _window_height, _window_title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(_window, this);

    _frame_buffer_size = GetWindowFrameBufferSize();

    RegisterEvents();

    _renderer = new Renderer(this);
    _gui = new GUI(this);

    v2d mouse_pos;
    glfwGetCursorPos(_window, &mouse_pos.x, &mouse_pos.y);
    _io_manager->MouseEvent(mouse_pos.x, mouse_pos.y);
    _io_manager->UpdateRelativePositions();
  }

  Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();

    delete _gui;
    delete _renderer;
    delete _io_manager;
  }

  IOManager &Window::GetIOManager() const {
    return *_io_manager;
  }

  bool Window::Update() {
    _io_manager->UpdatePads(_window);
    _io_manager->UpdateHolds();
    _io_manager->UpdateRelativePositions();

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

  Renderer &Window::GetRenderer() const {
    return *_renderer;
  }

  GUI &Window::GetGUI() const {
    return *_gui;
  }

  GLFWwindow *Window::GetGLFWWindow() const {
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
    _frame_buffer_size = GetWindowFrameBufferSize();
  }

  f64 Window::GetDeltaTime() const {
    return _delta_time;
  }

  f64 Window::GetTime() {
    return glfwGetTime();
  }

  void Window::RegisterEvents() {
    glfwSetKeyCallback(_window, Window::KeyEvent);
    glfwSetCharCallback(_window, Window::CharEvent);
    glfwSetMouseButtonCallback(_window, Window::MouseButtonEvent);
    glfwSetScrollCallback(_window, Window::ScrollEvent);
    glfwSetCursorPosCallback(_window, Window::MouseEvent);

    glfwSetWindowFocusCallback(_window, Window::FocusEvent);
    glfwSetCursorEnterCallback(_window, Window::CursorEnterEvent);
    glfwSetMonitorCallback(Window::MonitorEvent);
    glfwSetFramebufferSizeCallback(_window, FrameSizeEvent);
  }

  // Static Input Events

  void Window::KeyEvent(GLFWwindow *glfw_window, i32 key, i32 scancode, i32 action, i32 mods) {
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    if (action != GLFW_REPEAT)
      window->_io_manager->KeyEvent(key, action != GLFW_RELEASE);

    // if (window->lockMouse)
    //   return;

    window->_gui->KeyCallback(glfw_window, key, scancode, action, mods);
  }

  void Window::CharEvent(GLFWwindow *glfw_window, u32 c) {
    // if (window->lockMouse)
    //   return;

    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->_gui->CharCallback(glfw_window, c);
  }

  void Window::MouseButtonEvent(GLFWwindow *glfw_window, i32 button, i32 action, i32 mods) {
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->_io_manager->MouseButtonEvent(button, action != GLFW_RELEASE);

    // if (window->lockMouse)
    //   return;

    window->_gui->MouseButtonCallback(glfw_window, button, action, mods);
  }

  void Window::ScrollEvent(GLFWwindow *glfw_window, f64 xoffset, f64 yoffset) {
    // if (window->lockMouse)
    //   return;
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));

    window->_io_manager->ScrollEvent({ xoffset, yoffset });
    window->_gui->ScrollCallback(glfw_window, xoffset, yoffset);
  }

  void Window::MouseEvent(GLFWwindow *glfw_window, f64 x, f64 y) {
    if (!glfwGetWindowAttrib(glfw_window, GLFW_HOVERED))
      return;
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->_io_manager->MouseEvent(x, y);
  }

  void Window::FrameSizeEvent(GLFWwindow *glfw_window, i32 width, i32 height) {
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->SetSize(width, height);
  }

  void Window::FocusEvent(GLFWwindow *glfw_window, i32 focused) {
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->_gui->WindowFocusCallback(glfw_window, focused);
  }

  void Window::CursorEnterEvent(GLFWwindow *glfw_window, i32 entered) {
    Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    window->_gui->CursorEnterCallback(glfw_window, entered);
  }

  void Window::MonitorEvent(GLFWmonitor *monitor, i32 event) {
    GUI::MonitorCallback(monitor, event);
  }

  v2i Window::GetSize() const {
    return v2i(_window_width, _window_height);
  }

  v2i Window::GetWindowFrameBufferSize() const {
    v2i result;
    glfwGetFramebufferSize(_window, &result.x, &result.y);
    return result;
  }

  const v2i &Window::GetFrameBufferSize() const {
    return _frame_buffer_size;
  }

  void Window::SetFrameBufferSize(const v2i &size) {
    _frame_buffer_size = size;
  }

  void Window::LockMouse(bool state) {
    _lock_mouse = state;

    glfwSetInputMode(_window, GLFW_CURSOR, state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    _gui->LockMouse(state);
  }

  bool Window::GetLockMouse() const {
    return _lock_mouse;
  }

} // namespace axl
