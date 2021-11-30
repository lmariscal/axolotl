#include <axolotl/ento.h>

#include <axolotl/scene.h>

namespace axl {

  uuids::uuid_random_generator Ento::_uuid_generator(nullptr);

  Ento::Ento(entt::entity handle, Scene *scene):
    handle(handle),
    scene(scene)
  {
    if (_first_gen) {
      std::random_device rd;
      std::array<int, std::mt19937::state_size> _seed_data;
      std::generate(std::begin(_seed_data), std::end(_seed_data), std::ref(rd));
      std::seed_seq seq(std::begin(_seed_data), std::end(_seed_data));

      static std::mt19937 random_generator = std::mt19937(seq);
      _uuid_generator = uuids::uuid_random_generator(random_generator);
      _first_gen = false;
    }

    id = _uuid_generator();
    _ento_map.insert(std::make_pair(id, handle));
  }

  Ento::~Ento() {
    log::debug("Deleting ento {}", uuids::to_string(id));
  }

  void Ento::Destroy() {
    if (parent != entt::null) {
      Ento &ento_p = scene->GetComponent<Ento>(parent);
      ento_p.RemoveChild(entity);
    }

    _ento_map.erase(id);

    for (entt::entity child : children) {
      Ento &ento_c = scene->GetComponent<Ento>(child);
      ento_c.parent = entt::null;
    }

    for (Component *c : components)
      c->_parent = entt::null;

    entity = entt::null;
  }

  entt::entity Ento::FromID(uuids::uuid id) {
    return _ento_map[id];
  }

  void Ento::AddChild(entt::entity e) {
    children.push_back(e);
    Ento &ento_c = scene->GetComponent<Ento>(e);
    ento_c.parent = entity;
  }

  void Ento::RemoveChild(entt::entity e) {
    children.erase(std::remove(children.begin(), children.end(), e), children.end());
    Ento &ento_c = scene->GetComponent<Ento>(e);
    ento_c.parent = entt::null;
  }

  json Ento::Serialize() const {
    json j;
    j["name"] = name;
    j["id"] = uuids::to_string(id);
    if (parent != entt::null) {
      Ento &ento_p = scene->GetComponent<Ento>(parent);
      j["parent"] = uuids::to_string(ento_p.id);
    }

    if (!components.empty()) {
      j["components"] = json::array();
      for (auto &c : components)
        j["components"].push_back(c->Serialize());
    }

    if (!children.empty()) {
      j["children"] = json::array();
      for (auto &c : children) {
        Ento &ento_c = scene->GetComponent<Ento>(c);
        j["children"].push_back(uuids::to_string(ento_c.id));
      }
    }

    return j;
  }

} // namespace axl
