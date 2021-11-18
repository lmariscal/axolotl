#pragma once

#include <axolotl/types.h>

class GLFWwindow;

namespace axl {

  class Renderer;
  class GUI;

  class Window {
   public:
    Window(f32 width, f32 height, const std::string &title);
    ~Window();

    bool Update();
    void Draw();
    void SetTitle(const std::string &title);
    void SetSize(f32 width, f32 height);
    v2i GetSize() const;
    GLFWwindow *GetGLFWWindow() const;
    Renderer *GetRenderer() const;

   protected:
    friend class Renderer;

    f32 _window_height;
    f32 _window_width;
    std::string _window_title;
    Renderer *_renderer;
    GUI *_gui;
    GLFWwindow *_window;
  };

} // namespace axolotl
