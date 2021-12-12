#pragma once

#include "dockspace.hh"

#include <axolotl/ento.hh>
#include <axolotl/types.hh>

namespace axl {

  class Scene;
  class FrameEditor;

  class Inspector {
   public:
    Inspector();

    void Draw(Scene &scene, DockSpace &dock);

   protected:
    friend class FrameEditor;

    void ShowAddComponent(Scene &scene);
    void ShowComponents(Ento &ento, Scene &scene);

    std::string _model_path;
    bool _add_model = false;
    bool _want_model = false;
    bool _edit_name = false;
    bool _edit_name_first = false;
    uuid _selected_entity_id;
  };

} // namespace axl
