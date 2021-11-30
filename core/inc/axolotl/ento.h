#pragma once

#include <axolotl/types.h>

#include <uuid.h>
#include <array>
#include <random>
#include <vector>

#include <axolotl/component.h>
#include <axolotl/scene.h>

namespace axl {

  struct Tag {
    std::string value;

    Tag(const std::string &value): value(value) { }
  };

  struct Ento {
    Ento(): handle(entt::null), scene(nullptr) { }
    Ento(entt::entity handle, Scene *scene);
    ~Ento();

    void AddChild(entt::entity e);
    void RemoveChild(entt::entity e);
    json Serialize() const;

    template<typename... Components>
    bool HasAllOf() const {
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
      asssert(!HasAllOf<T>(), "Entity already has component");

      return scene->_registry.emplace<T>(handle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& GetComponent() {
      asssert(HasAllOf<T>(), "Entity does not have component");

      return scene->_registry.get<T>(handle);
    }

    template<typename T>
    const T& GetComponent() const {
      asssert(HasAllOf<T>(), "Entity does not have component");

      return scene->_registry.get<T>(handle);
    }

    template<typename T>
    void RemoveComponent() {
      asssert(HasAllOf<T>(), "Entity does not have component");

      scene->_registry.remove<T>(handle);
    }

    operator bool() const {
      return handle != entt::null;
    }

    static Ento & FromID(uuids::uuid id);

    uuids::uuid id;
    entt::entity handle;
    Scene *scene;

    bool marked_for_deletion;

    uuids::uuid parent;
    std::vector<uuids::uuid> children;

   protected:
    static uuids::uuid_random_generator _uuid_generator;
    inline static bool _first_gen = true;
    inline static std::map<uuids::uuid, entt::entity> _ento_map;
  };

} // namespace axl
