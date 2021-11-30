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

  entt::entity Scene::CreateEntity() {
    entt::entity e = _registry.create();
    Ento &ento = _registry.emplace<Ento>(e, e, *this);
    log::debug("Created entity with id {}", uuids::to_string(ento.id));
    return e;
  }

  void Scene::RemoveEntity(entt::entity e) {
    if (!_registry.valid(e)) {
      log::error("Trying to remove invalid entity {}", e);
      return;
    }

    Ento *ento = _registry.try_get<Ento>(e);

    for (Component *c : ento->components) {
      c->Destroy(ento);
      c->_parent = entt::null;
    }

    for (entt::entity c : ento->children)
      RemoveEntity(c);

    ento->Destroy();

    _registry.destroy(e);
    log::debug("Removed entity with id {}", uuids::to_string(ento->id));
  }

  void Scene::Draw(Renderer &renderer) {
    renderer.Render(_registry);
  }

  Scene * Scene::GetActiveScene() {
    return _active_scene;
  }

  json Scene::Serialize() {
    json j;
    j["entities"] = json::array();
    for (auto &e : _registry.view<Ento>()) {
      Ento &ento = _registry.get<Ento>(e);
      j["entities"].push_back(ento.Serialize());
    }
    return j;
  }

}
