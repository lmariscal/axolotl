#include <axolotl/ento.h>

namespace axl {

  uuids::uuid_random_generator Ento::_uuid_generator(nullptr);

  Ento::Ento():
    parent(nullptr)
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

    name.reserve(64);
    id = _uuid_generator();

    _ento_map.insert(std::make_pair(id, this));
  }

  Ento::~Ento() {
    if (parent)
      parent->RemoveChild(this);

    _ento_map.erase(id);
  }

  void Ento::AddChild(Ento *ento) {
    children.push_back(ento);
    ento->parent = this;
  }

  void Ento::RemoveChild(Ento *ento) {
    children.erase(std::remove(children.begin(), children.end(), ento), children.end());
    ento->parent = nullptr;
  }

  json Ento::Serialize() const {
    json j;
    j["name"] = name;
    j["id"] = uuids::to_string(id);
    if (parent)
      j["parent"] = uuids::to_string(parent->id);

    if (!components.empty()) {
      j["components"] = json::array();
      for (auto &c : components)
        j["components"].push_back(c->Serialize());
    }

    if (!children.empty()) {
      j["children"] = json::array();
      for (auto &c : children)
        j["children"].push_back(uuids::to_string(id));
    }

    return j;
  }

} // namespace axl
