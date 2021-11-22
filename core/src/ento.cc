#include <axolotl/ento.h>

namespace axl {

  bool Ento::_first_gen = true;
  std::mt19937 Ento::_random_generator;
  uuids::uuid_random_generator Ento::_uuid_generator(nullptr);

  Ento::Ento() {
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
  }

} // namespace axl
