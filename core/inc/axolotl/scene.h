#pragma once

#include <axolotl/types.h>
#include <entt/entt.hpp>

namespace axl {

  class Scene {
   public:
     Scene();
     ~Scene();

     virtual void Init() = 0;
     virtual void Update(f32 delta) = 0;

     entt::registry * GetRegistry();
     void Draw();

   protected:
    entt::registry _registry;
  };

} // namespace axl
