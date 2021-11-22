#pragma once

#include <axolotl/types.h>
#include <entt/entt.hpp>

namespace axl {

  class Scene;
  class FrameEditor;

  class Inspector {
   public:

    void Draw(Scene &scene);

   protected:
    friend class FrameEditor;

    entt::entity _selected_entity = entt::null;
  };

} // namespace axl
