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

   protected:
    friend class GUI;

    Window *_window;
  };

} // namespace axl
