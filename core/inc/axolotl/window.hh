#pragma once

#include <axolotl/iomanager.hh>
#include <axolotl/types.hh>

struct GLFWwindow;
struct GLFWmonitor;

namespace axl {

  class Renderer;
  class GUI;

  class Window {
   public:
    Window(u32 width, u32 height, const std::string &title);
    ~Window();

    bool GetLockMouse() const;
    bool Update();
    void Draw();
    void SetTitle(const std::string &title);
    void SetSize(u32 width, u32 height);
    void RegisterEvents();
    void LockMouse(bool state);
    void SetFrameBufferSize(const v2i &size);
    v2i GetSize() const;
    v2i GetWindowFrameBufferSize() const;
    const v2i &GetFrameBufferSize() const;
    // Returns Delta time in milliseconds
    f64 GetDeltaTime() const;
    static f64 GetTime();
    Renderer &GetRenderer() const;
    IOManager &GetIOManager() const;
    GUI &GetGUI() const;
    GLFWwindow *GetGLFWWindow() const;

    static inline Window *GetCurrentWindow() {
      return _active_window;
    }

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
    v2i _frame_buffer_size;
    std::string _window_title;
    GUI *_gui;
    Renderer *_renderer;
    IOManager *_io_manager;
    GLFWwindow *_window;
    bool _lock_mouse;

    inline static Window *_active_window;
  };

} // namespace axl
