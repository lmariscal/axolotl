#pragma once

#include <axolotl/types.h>
#include <axolotl/iomanager.h>

class GLFWwindow;
class GLFWmonitor;

namespace axl {

  class Renderer;
  class GUI;

  class Window {
   public:
    Window(u32 width, u32 height, const std::string &title);
    ~Window();

    bool Update();
    void Draw();
    void SetTitle(const std::string &title);
    void SetSize(u32 width, u32 height);
    void RegisterEvents();
    v2i GetSize() const;
    // Returns Delta time in milliseconds
    f64 GetDeltaTime() const;
    Renderer * GetRenderer() const;
    IOManager * GetIOManager() const;
    GLFWwindow * GetGLFWWindow() const;

   protected:
    friend class Renderer;

    static inline void KeyEvent(GLFWwindow *win, i32 key, i32 scancode, i32 action, i32 mods);
    static inline void CharEvent(GLFWwindow *window, u32 c);
    static inline void MouseButtonEvent(GLFWwindow *window, i32 button, i32 action, i32 mods);
    static inline void ScrollEvent(GLFWwindow *window, double xoffset, double yoffset);
    static inline void MouseEvent(GLFWwindow *window, f64 x, f64 y);
    static inline void FrameSizeEvent(GLFWwindow *window, i32 width, i32 height);
    static inline void FocusEvent(GLFWwindow *window, i32 focused);
    static inline void CursorEnterEvent(GLFWwindow *window, i32 entered);
    static inline void MonitorEvent(GLFWmonitor *monitor, i32 event);

    f64 _time_last;
    f64 _delta_time;
    u32 _window_height;
    u32 _window_width;
    std::string _window_title;
    GUI *_gui;
    Renderer *_renderer;
    IOManager *_io_manager;
    GLFWwindow *_window;
  };

} // namespace axolotl
