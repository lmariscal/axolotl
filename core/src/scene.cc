#include <axolotl/scene.h>

#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <axolotl/mesh.h>

namespace axl {

  Scene::Scene() {
    _active_scene = this;
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
    Ento *ento = _registry.try_get<Ento>(e);

    for (Component *c : ento->components)
      c->Destroy();

    for (Ento *c : ento->children)
      RemoveEntity(*c);

    _registry.destroy(e);
    log::debug("Removed entity with id {}", uuids::to_string(ento->id));
  }

  void Scene::Draw(Renderer &renderer) {
    renderer.Render(_registry);
  }

  Scene * Scene::GetActiveScene() {
    return _active_scene;
  }

}
