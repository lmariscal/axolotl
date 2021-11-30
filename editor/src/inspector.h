#pragma once

#include <axolotl/types.h>
#include <axolotl/ento.h>

namespace axl {

  class Scene;
  class FrameEditor;

  class Inspector {
   public:
    Inspector();

    void Draw(Scene &scene);

   protected:
    friend class FrameEditor;

    void ShowAddComponent(Scene &scene);

    std::string _model_path;
    bool _add_model = false;
    bool _want_model = false;
    Ento _selected_entity;
  };

} // namespace axl
