#pragma once

#include <axolotl/types.h>
#include <entt/entt.hpp>

namespace axl {

  class Renderer;
  class Window;

  class Scene {
   public:
     Scene();
     ~Scene();

     virtual void Init() = 0;
     virtual void Update(Window &window) = 0;

     entt::registry * GetRegistry();
     entt::entity CreateEntity();
     void Draw(Renderer &renderer);

   protected:
    entt::registry _registry;
  };

} // namespace axl
