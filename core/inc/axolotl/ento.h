#pragma once

#include <axolotl/types.h>

#include <uuid.h>
#include <array>
#include <random>
#include <vector>

#include <axolotl/component.h>
#include <axolotl/transform.h>
#include <axolotl/scene.h>

namespace axl {

  struct HierarchyComponent {
    uuid parent;
    std::vector<uuid> children;
  };

  struct Tag {
    std::string value;

    Tag(const std::string &value): value(value) { }
    Tag(): value("entity") {  }
  };

  struct Ento {
    Ento(): handle(entt::null), scene(nullptr), id(uuid()) { }
    Ento(entt::entity handle, Scene *scene);
    ~Ento();

    json Serialize() const;

    template<typename... Components>
    bool HasComponent() const {
      return scene->_registry.all_of<Components...>(handle);
    }

    template<typename... Components>
    bool HasAnyOf() const {
      return scene->_registry.any_of<Components...>(handle);
    }

    bool IsOrphan() const {
      return !scene->_registry.orphan(handle);
    }

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args) {
      AXL_ASSERT(!HasComponent<T>(), "Entity already has component");

      return scene->_registry.emplace<T>(handle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    T& TryAddComponent(Args&&... args) {
      if (HasComponent<T>())
        return GetComponent<T>();
      return scene->_registry.emplace<T>(handle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& GetComponent() {
      AXL_ASSERT(HasComponent<T>(), "Entity does not have component");

      return scene->_registry.get<T>(handle);
    }

    template<typename T>
    const T& GetComponent() const {
      AXL_ASSERT(HasComponent<T>(), "Entity does not have component");

      return scene->_registry.get<T>(handle);
    }

    template<typename T>
    void RemoveComponent() {
      AXL_ASSERT(HasComponent<T>(), "Entity does not have component");

      scene->_registry.remove<T>(handle);
    }

    operator bool() const {
      return handle != entt::null;
    }

    operator entt::entity() const {
      return handle;
    }

    operator uuid() const {
      return id;
    }

    bool operator==(const Ento &other) const {
      return handle == other.handle;
    }

    bool operator!=(const Ento &other) const {
      return handle != other.handle;
    }

    Transform & Transform();
    const struct Transform & Transform() const;

    Tag & Tag();
    const struct Tag & Tag() const;

    bool HasParent() const;
    bool HasChildren() const;
    Ento Parent();
    void SetParent(Ento parent);
    void AddChild(Ento child);
    void RemoveChild(Ento child);
    std::vector<Ento> Children();

    uuid id;
    entt::entity handle;
    Scene *scene;

   protected:
    friend class Scene;

    static uuids::uuid_random_generator _uuid_generator;

    inline static bool _first_gen = true;
    inline static std::map<uuid, Ento> _uuid_ento_map;
    inline static std::map<entt::entity, Ento> _handle_ento_map;
  };

} // namespace axl
