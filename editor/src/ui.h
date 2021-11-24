#include <axolotl/framebuffer.h>

#include <axolotl/types.h>
#include <axolotl/renderer.h>
#include <entt/entt.hpp>

#include "inspector.h"

namespace axl {

  class Scene;
  struct TerminalData;

  class FrameEditor {
   public:
    FrameEditor();
    ~FrameEditor();

    void Bind(Window &window);
    void Unbind(Window &window);
    void Draw(Window &window, TerminalData &data);
    void SetBoundFrameRatio(bool state);
    void DrawEntityList(Scene &scene);
    void DrawInspector(Scene &scene);
    const v2 & GetRegionAvailable() const;

    bool bound_frame_ratio;
    bool frame_focused;
    bool fullscreen_play;

   protected:
    FrameBuffer _frame;
    Inspector _inspector;

    v2 _region_available;
  };

} // namespace axl