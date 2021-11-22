#include <axolotl/framebuffer.h>

#include <axolotl/types.h>
#include <axolotl/renderer.h>
#include <entt/entt.hpp>

namespace axl {

  class Scene;

  class FrameEditor {
   public:
    FrameEditor();
    ~FrameEditor();

    void Bind(Window &window);
    void Unbind(Window &window);
    void Draw(Window &window);
    void SetBoundFrameRatio(bool state);
    void DrawEntityList(Scene &scene);
    void DrawInspector(Scene &scene);

    bool bound_frame_ratio;

   protected:
    FrameBuffer _frame;
    v2 _region_available;

    entt::entity _selected_entity = entt::null;
  };

} // namespace axl
