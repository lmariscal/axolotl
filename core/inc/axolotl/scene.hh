#pragma once

#include <axolotl/types.hh>

#include <entt/entt.hpp>

namespace axl {

  class Renderer;
  class Window;

  struct Ento;

  class Scene {
   public:
    virtual void Init() = 0;
    virtual void Update(Window &window) = 0;
    virtual void Focused(Window &window, bool stat) = 0;

    Ento CreateEntity(const std::string &name = "");
    void RemoveEntity(Ento ento);
    Ento FromID(uuid id);
    Ento FromHandle(entt::entity handle);
    entt::registry & GetRegistry();

    void Draw(Renderer &renderer, bool show_data = false);
    json Serialize();

    static void SetActiveScene(Scene *scene);
    static Scene * GetActiveScene();

   protected:
    friend class Ento;
    friend class FrameEditor;

    template<typename T>
    T & GetComponent(entt::entity handle) {
      return _registry.get<T>(handle);
    }

    template<typename T>
    T & GetComponent(entt::entity handle) const {
      return _registry.get<T>(handle);
    }

    entt::registry _registry;

    static inline Scene *_active_scene;
  };

} // namespace axl
