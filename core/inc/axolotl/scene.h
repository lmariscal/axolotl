#pragma once

#include <entt/entt.hpp>

namespace axl {

  class Scene {
   public:
     Scene();
     ~Scene();

   protected:
    entt::registry _registry;
    entt::entity _root;
  };

} // namespace axl
