#include <axolotl/scene.h>

#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <axolotl/mesh.h>
#include <axolotl/ento.h>
#include <axolotl/transform.h>

namespace axl {

  void Scene::SetActiveScene(Scene *scene) {
    _active_scene = scene;
  }

  Ento Scene::CreateEntity(const std::string &name) {
    Ento e(_registry.create(), this);
    e.AddComponent<Transform>();
    e.AddComponent<HierarchyComponent>();

    Tag &tag = e.AddComponent<Tag>();
    name.empty() ? tag.value = "entity" : tag.value = name;

    Ento::_uuid_ento_map.insert(std::make_pair(e.id, e));
    Ento::_handle_ento_map.insert(std::make_pair(e.handle, e));

    log::debug("Created ento {}", uuids::to_string(e.id));

    return e;
  }

  void Scene::RemoveEntity(Ento e) {
    AXL_ASSERT(_registry.valid(e.handle), "Trying to remove invalid entity {}->{}", e.handle, uuids::to_string(e.id));

    if (e.HasChildren())
      for (auto &child : e.Children())
        RemoveEntity(child);

    Ento::_uuid_ento_map.erase(e.id);
    Ento::_handle_ento_map.erase(e.handle);

    _registry.destroy(e.handle);
    log::debug("Removed entity with id {}", uuids::to_string(e.id));
  }

  Ento Scene::FromID(uuid id) {
    if (!Ento::_uuid_ento_map.count(id))
      return { };

    return Ento::_uuid_ento_map[id];
  }

  Ento Scene::FromHandle(entt::entity handle) {
    if (!Ento::_handle_ento_map.count(handle))
      return { };

    return Ento::_handle_ento_map[handle];
  }

  void Scene::Draw(Renderer &renderer) {
    renderer.Render(*this);
  }

  Scene * Scene::GetActiveScene() {
    return _active_scene;
  }

  entt::registry & Scene::GetRegistry() {
    return _registry;
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
