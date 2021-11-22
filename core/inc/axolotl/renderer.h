#pragma once

#include <axolotl/types.h>
#include <entt/entt.hpp>

namespace axl {

  class Window;
  class GUI;

  class Renderer {
   public:
    Renderer(Window *window);
    ~Renderer();

    void ClearScreen(const v3 &color);
    void Resize(u32 width, u32 height);

    void Render(entt::registry &registry);

   protected:
    friend class GUI;

    Window *_window;
  };

} // namespace axl
