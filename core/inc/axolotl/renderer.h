#pragma once

#include <axolotl/types.h>
#include <axolotl/scene.h>
#include <entt/entt.hpp>

namespace axl {

  class Window;
  class GUI;

  class Renderer {
   public:
    Renderer(Window *window);

    void ClearScreen(const v3 &color);
    void Resize(u32 width, u32 height);

    void Render(Scene &scene, bool show_data);
    void SetMeshWireframe(bool state);

   protected:
    friend class GUI;

    void ShowData();

    f64 _delta_time;
    f64 _delta_time_accum;
    f64 _last_time;
    u32 _fps;
    u32 _frame_count;

    Window *_window;
  };

} // namespace axl
