#include <axolotl/ento.hh>
#include <axolotl/model.hh>
#include <axolotl/transform.hh>

namespace axl {

  uuids::uuid_random_generator Ento::_uuid_generator(nullptr);

  Ento::Ento(entt::entity handle, Scene *scene): handle(handle), scene(scene) {
    if (_first_gen) {
      std::random_device rd;
      std::array<int, std::mt19937::state_size> _seed_data;
      std::generate(std::begin(_seed_data), std::end(_seed_data), std::ref(rd));
      std::seed_seq seq(std::begin(_seed_data), std::end(_seed_data));

      _random_generator = std::mt19937(seq);
      _uuid_generator = uuids::uuid_random_generator(_random_generator);
      _first_gen = false;
      log::debug("Random generator seeded");
    }

    id = _uuid_generator();
    log::debug("Created ento {}", uuids::to_string(id));
  }

  Ento::~Ento() {
    if (id.is_nil())
      return;
  }

  void Ento::AddChild(Ento child) {
    HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    hierarchy.children.push_back(child.id);
    child.SetParent(*this);
  }

  void Ento::RemoveChild(Ento child) {
    HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    hierarchy.children.erase(std::remove(hierarchy.children.begin(), hierarchy.children.end(), child),
                             hierarchy.children.end());
    child.SetParent({});
  }

  Transform &Ento::Transform() {
    return GetComponent<class Transform>();
  }

  const Transform &Ento::Transform() const {
    return GetComponent<class Transform>();
  }

  Tag &Ento::Tag() {
    return GetComponent<class Tag>();
  }

  const Tag &Ento::Tag() const {
    return GetComponent<class Tag>();
  }

  Ento Ento::Parent() {
    HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    if (hierarchy.parent.is_nil())
      return {};
    return scene->FromID(hierarchy.parent);
  }

  bool Ento::HasParent() const {
    const HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    return !hierarchy.parent.is_nil();
  }

  bool Ento::HasChildren() const {
    const HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    return !hierarchy.children.empty();
  }

  void Ento::SetParent(Ento parent) {
    HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    hierarchy.parent = parent.id;
  }

  std::vector<Ento> Ento::Children() {
    HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    std::vector<Ento> children;
    for (auto id : hierarchy.children) {
      children.push_back(scene->FromID(id));
    }
    return children;
  }

  json Ento::Serialize() const {
    json j;
    j["id"] = uuids::to_string(id);
    j["name"] = Tag().value;
    const HierarchyComponent &hierarchy = GetComponent<HierarchyComponent>();
    j["parent"] = uuids::to_string(hierarchy.parent);
    j["children"] = json::array();
    for (auto id : hierarchy.children)
      j["children"].push_back(uuids::to_string(id));

    j["components"] = json::array();
    j["components"].push_back(Transform().Serialize());

    if (HasComponent<Model>())
      j["components"].push_back(GetComponent<Model>().Serialize());

    return j;
  }

  void Ento::ClearMaps() {
    _uuid_ento_map.clear();
    _handle_ento_map.clear();
  }

} // namespace axl
