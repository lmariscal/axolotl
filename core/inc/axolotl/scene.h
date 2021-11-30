#pragma once

#include <axolotl/types.h>
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
    void RemoveEntity(Ento &ento);

    void Draw(Renderer &renderer);
    json Serialize();

    static void SetActiveScene(Scene *scene);
    static std::shared_ptr<Scene> GetActiveScene();

   protected:
    friend class Ento;

    entt::registry _registry;

    static inline std::shared_ptr<Scene> _active_scene;
  };

} // namespace axl
