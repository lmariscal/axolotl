#include <axolotl/scene.h>

#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <axolotl/mesh.h>
#include <axolotl/ento.h>
#include <axolotl/transform.h>

namespace axl {

  void Scene::SetActiveScene(Scene *scene) {
    _active_scene = std::make_shared<Scene>(scene);
  }

  Ento Scene::CreateEntity(const std::string &name) {
    Ento e(_registry.create(), this);
    e.AddComponent<Transform>();
    Tag &tag = e.AddComponent<Tag>();
    name.empty() ? tag.value = "Entity" : tag.value = name;
    return e;
  }

  void Scene::RemoveEntity(Ento &e) {
    if (!_registry.valid(e.handle)) {
      log::error("Trying to remove invalid entity {}->{}", e.handle, e.id);
      return;
    }

    _registry.destroy(e.handle);
    log::debug("Removed entity with id {}", uuids::to_string(e.id));
  }

  void Scene::Draw(Renderer &renderer) {
    renderer.Render(_registry);
  }

  std::shared_ptr<Scene> Scene::GetActiveScene() {
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
