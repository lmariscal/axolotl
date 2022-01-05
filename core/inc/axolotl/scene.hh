#pragma once

#include <axolotl/types.hh>
#include <entt/entt.hpp>

namespace axl {

  class Renderer;
  class Window;
  class Camera;
  class Transform;

  class Ento;

  class Scene {
   public:
    virtual ~Scene() = default;
    virtual void Init(Window &window) = 0;
    virtual void Update(Window &window, f64 delta) = 0;
    virtual void Focused(Window &window, bool stat) = 0;

    Ento CreateEntity(const std::string &name = "");
    void RemoveEntity(Ento ento);
    Ento FromID(uuid id);
    Ento FromHandle(entt::entity handle);
    entt::registry &GetRegistry();

    void PhysicsUpdate(f64 step);
    void
    Draw(Renderer &renderer, bool show_data = false, Camera *camera = nullptr, Transform *camera_transform = nullptr);
    json Serialize();
    void Deserialize(const json &data);

    static void SetActiveScene(Scene *scene);
    static Scene *GetActiveScene();

   protected:
    friend class Ento;
    friend class FrameEditor;

    template<typename T>
    T &GetComponent(entt::entity handle) {
      return _registry.get<T>(handle);
    }

    template<typename T>
    T &GetComponent(entt::entity handle) const {
      return _registry.get<T>(handle);
    }

    entt::registry _registry;
    bool focused = false;

    static inline Scene *_active_scene;
  };

} // namespace axl
