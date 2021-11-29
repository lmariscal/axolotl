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
    entt::entity e = _registry.create();
    Ento &ento = _registry.emplace<Ento>(e);
    ento.entity = e;
    log::debug("Created entity with id {}", uuids::to_string(ento.id));
    return e;
  }

  void Scene::RemoveEntity(entt::entity e) {
    ENTT_ASSERT(_registry.valid(e), "Invalid entity");
    std::string id = uuids::to_string(_registry.get<Ento>(e).id);
    _registry.destroy(e);
    log::debug("Removed entity with id {}", id);
  }

  void Scene::Draw(Renderer &renderer) {
    renderer.Render(_registry);
  }

}
