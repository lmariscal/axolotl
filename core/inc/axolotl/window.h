#pragma once

#include <axolotl/types.h>

class GLFWwindow;

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
    v2i GetSize() const;
    // Returns Delta time in milliseconds
    f64 GetDeltaTime() const;
    GLFWwindow *GetGLFWWindow() const;
    Renderer *GetRenderer() const;

   protected:
    friend class Renderer;

    f64 _time_last;
    f64 _delta_time;
    u32 _window_height;
    u32 _window_width;
    std::string _window_title;
    Renderer *_renderer;
    GUI *_gui;
    GLFWwindow *_window;
  };

} // namespace axolotl
