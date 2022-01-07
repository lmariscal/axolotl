#pragma once

#include <array>
#include <axolotl/component.hh>
#include <axolotl/scene.hh>
#include <axolotl/transform.hh>
#include <axolotl/types.hh>
#include <random>
#include <uuid.h>
#include <vector>

namespace axl {

  class HierarchyComponent {
   public:
    void Init() { }

    uuid parent;
    std::vector<uuid> children;

    REGISTER_COMPONENT(HierarchyComponent, parent, children)
  };

  class Tag {
   public:
    static constexpr std::string_view DefaultTag = "Entity";

    void Init() { }

    std::string value;

    REGISTER_COMPONENT(Tag, value)

    Tag(const std::string &value): value(value) { }
    Tag(): value(DefaultTag) { }
  };

  class Ento {
   public:
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
    T &AddComponent(Args &&...args) {
      AXL_ASSERT_MESSAGE(!HasComponent<T>(), "Entity already has component");

      T &t = scene->_registry.emplace<T>(handle, std::forward<Args>(args)...);
      t.Init();
      return t;
    }

    template<typename T, typename... Args>
    T &TryAddComponent(Args &&...args) {
      if (HasComponent<T>())
        return GetComponent<T>();
      T &t = scene->_registry.emplace<T>(handle, std::forward<Args>(args)...);
      t.Init();
      return t;
    }

    template<typename T>
    T &GetComponent() {
      AXL_ASSERT_MESSAGE(HasComponent<T>(), "Entity does not have component");

      return scene->_registry.get<T>(handle);
    }

    template<typename T>
    const T &GetComponent() const {
      AXL_ASSERT_MESSAGE(HasComponent<T>(), "Entity does not have component");

      return scene->_registry.get<T>(handle);
    }

    template<typename T>
    void RemoveComponent() {
      AXL_ASSERT_MESSAGE(HasComponent<T>(), "Entity does not have component");

      scene->_registry.remove<T>(handle);
    }

    template<typename T>
    static Ento FromComponent(T &component) {
      Scene *scene = Scene::GetActiveScene();
      if (!scene)
        return Ento();
      entt::entity e = entt::to_entity(scene->GetRegistry(), component);
      return _handle_ento_map[e];
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

    Transform &Transform();
    const class Transform &Transform() const;

    Tag &Tag();
    const class Tag &Tag() const;

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

    static void ClearMaps();
    static uuids::uuid_random_generator _uuid_generator;
    inline static std::mt19937 _random_generator;

    inline static bool _first_gen = true;
    inline static std::map<uuid, Ento> _uuid_ento_map;
    inline static std::map<entt::entity, Ento> _handle_ento_map;
  };

} // namespace axl
