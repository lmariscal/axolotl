#include <axolotl/ento.h>

namespace axl {

  bool Ento::_first_gen = true;
  std::mt19937 Ento::_random_generator;
  uuids::uuid_random_generator Ento::_uuid_generator(nullptr);

  Ento::Ento():
    parent(nullptr)
  {
    if (_first_gen) {
      std::random_device rd;
      std::array<int, std::mt19937::state_size> _seed_data;
      std::generate(std::begin(_seed_data), std::end(_seed_data), std::ref(rd));
      std::seed_seq seq(std::begin(_seed_data), std::end(_seed_data));

      _random_generator = std::mt19937(seq);
      _uuid_generator = uuids::uuid_random_generator(_random_generator);
      _first_gen = false;
    }

    name.resize(64);
    id = _uuid_generator();
  }

  Ento::~Ento() {
    if (parent)
      parent->RemoveChild(this);
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
