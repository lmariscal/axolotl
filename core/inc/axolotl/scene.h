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
    virtual void Focused(Window &window, bool stat) = 0;

    entt::registry * GetRegistry();
    entt::entity CreateEntity();
    void RemoveEntity(const entt::entity entity);
    void Draw(Renderer &renderer);

    static Scene * GetActiveScene();

    template<typename ComponentType, typename... Args>
    decltype(auto) AddComponent(const entt::entity entity, Args &&...args) {
      ENTT_ASSERT(_registry.valid(entity), "Invalid entity");
      auto &c = _registry.emplace<ComponentType>(entity, std::forward<Args>(args)...);
      if constexpr (std::is_base_of_v<Component, ComponentType>) {
        Ento *ento = &_registry.get<Ento>(entity);
        ento->components.push_back(&c);
        c._parent = ento;
        c._scene = this;
        c.Init();
      }
      return c;
    }

    template<typename ComponentType, typename... Args>
    decltype(auto) TryAddComponent(const entt::entity entity, Args &&...args) {
      ENTT_ASSERT(_registry.valid(entity), "Invalid entity");
      auto *e = _registry.try_get<ComponentType>(entity);
      if (e)
        return _registry.get<ComponentType>(entity);

      return AddComponent<ComponentType>(entity, std::forward<Args>(args)...);
    }

    template<typename... ComponentType>
    [[nodiscard]] decltype(auto) GetComponent([[maybe_unused]] const entt::entity entity) {
        ENTT_ASSERT(_registry.valid(entity), "Invalid entity");
        return _registry.template get<ComponentType...>(entity);
    }

    template<typename... Component>
    [[nodiscard]] auto TryGetComponent([[maybe_unused]] const entt::entity entity) {
        ENTT_ASSERT(_registry.valid(entity), "Invalid entity");
        return _registry.try_get<Component...>(entity);
    }

   protected:
    entt::registry _registry;

    static inline Scene *_active_scene = nullptr;
  };

} // namespace axl
