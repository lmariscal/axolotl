#include <axolotl/framebuffer.hh>

#include <axolotl/types.hh>
#include <axolotl/renderer.hh>
#include <axolotl/ento.hh>
#include <entt/entt.hpp>

#include "inspector.hh"

namespace axl {

  class Scene;
  class DockSpace;

  enum class EditorAction {
    Select,
    Move,
    Rotate,
    Scale
  };

  class FrameEditor {
   public:
    FrameEditor();
    ~FrameEditor();

    void Bind(Window &window);
    void Unbind(Window &window);
    void Draw(Window &window, DockSpace &dock);
    void DrawEntityList(Scene &scene, DockSpace &dock);
    void DrawInspector(Scene &scene, DockSpace &dock);
    const v2 & GetRegionAvailable() const;

    bool focused;
    EditorAction action;

   protected:
    void ShowTreeEnto(Ento ento, u32 depth, Scene &scene);
    bool ShowEntityPopUp(Ento ento, Scene &scene);

    FrameBuffer _frame;
    Inspector _inspector;

    std::string _search_string;
    v2 _region_available;
  };

} // namespace axl
