#include <axolotl/scene.h>

#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <axolotl/mesh.h>

namespace axl {

  Scene::Scene() {
  }

  Scene::~Scene() {
  }

  entt::registry * Scene::GetRegistry() {
    return &_registry;
  }

  entt::entity Scene::CreateEntity() {
    return _registry.create();
  }

  void Scene::Draw(Renderer &renderer) {
    renderer.Render(_registry);
  }

}
