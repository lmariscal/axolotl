#include "inspector.hh"

#include <axolotl/ento.hh>
#include <axolotl/framebuffer.hh>
#include <axolotl/renderer.hh>
#include <axolotl/types.hh>
#include <entt/entt.hpp>

namespace axl {

  class Scene;
  class DockSpace;

  enum class EditorAction { Select, Move, Rotate, Scale };

  class FrameEditor {
   public:
    FrameEditor();
    ~FrameEditor();

    static bool CompareEntoID(const Ento &a, const Ento &b);

    void Bind(Window &window);
    void Unbind(Window &window);
    void Draw(Window &window, DockSpace &dock, Camera *camera);
    void DrawActionButtons(Window &window, DockSpaceData &dock_space_data);
    void DrawEntityList(Scene &scene, DockSpace &dock);
    void DrawInspector(Scene &scene, DockSpace &dock);
    void DrawGuizmo(Window &window, Camera *camera);
    const std::set<Ento, decltype(CompareEntoID) *>
    FilterEntities(Scene &scene, const std::vector<Ento> &entities, bool root);
    const v2 &GetRegionAvailable() const;

    bool focused;
    EditorAction action;

   protected:
    void ShowTreeEnto(Ento ento, u32 depth, Scene &scene, const std::set<Ento, decltype(CompareEntoID) *> &entities);
    bool ShowEntityPopUp(Ento ento, Scene &scene);

    FrameBuffer _frame;
    Inspector _inspector;

    std::string _search_string;
    v2 _region_available;
    v2 _region_cursor;

    json _pre_play_state;
  };

} // namespace axl
