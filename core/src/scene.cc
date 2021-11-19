#include <axolotl/scene.h>

#include <axolotl/shader.h>
#include <axolotl/mesh.h>

namespace axl {

  Scene::Scene() {
  }

  Scene::~Scene() {
  }

  entt::registry * Scene::GetRegistry() {
    return &_registry;
  }

  void Scene::Draw() {
    auto view = _registry.view<Mesh, Shader>();
    for (auto entity : view) {
      Mesh &mesh = view.get<Mesh>(entity);
      Shader &shader = view.get<Shader>(entity);
      shader.Bind();
      mesh.Draw();
    }
  }

}
