#pragma once

#include <axolotl/types.h>

namespace axl {

  class Window;
  class GUI;

  class Renderer {
   public:
    Renderer(Window *window);
    ~Renderer();

    void ClearScreen(const v3 &color);
    void Resize(u32 width, u32 height);

   protected:
    friend class GUI;

    Window *_window;
  };

} // namespace axl
