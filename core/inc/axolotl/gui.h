#pragma once

#include <axolotl/types.h>

class GLFWwindow;
class GLFWmonitor;

namespace axl {

  class Renderer;
  class Window;

  class GUI {
   public:
    GUI(Renderer *renderer, Window *window);
    ~GUI();

    void Update();
    void Draw();

   protected:
    friend class Renderer;
    friend class Window;

    void WindowFocusCallback(GLFWwindow* window, i32 focused);
    void CursorEnterCallback(GLFWwindow* window, i32 entered);
    void MouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods);
    void ScrollCallback(GLFWwindow* window, f64 x_offset, f64 y_offset);
    void KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
    void CharCallback(GLFWwindow* window, u32 character);

    static void MonitorCallback(GLFWmonitor* monitor, i32 event);

    Renderer *_renderer;
  };

} // namespace axl
