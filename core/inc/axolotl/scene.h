#pragma once

#include <axolotl/types.h>
#include <entt/entt.hpp>
#include <axolotl/ento.h>

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

    template<typename Component, typename... Args>
    decltype(auto) AddComponent(const entt::entity entity, Args &&...args) {
      auto &c = _registry.emplace<Component>(entity, std::forward<Args>(args)...);

      if constexpr (std::is_base_of_v<Serializable, Component>)
        _registry.get<Ento>(entity).components.push_back(&c);

      return c;
    }

   protected:
    entt::registry _registry;
  };

} // namespace axl
