#pragma once

#include <axolotl/types.h>

#include <uuid.h>
#include <array>
#include <random>

#include <axolotl/serialization.h>

namespace axl {

  struct Ento {
    Ento();
    ~Ento();

    uuids::uuid id;
    std::string name;

    Ento *parent;
    std::vector<Ento *> children;
    std::vector<Serializable *> components;

   protected:
    static std::mt19937 _random_generator;
    static uuids::uuid_random_generator _uuid_generator;
    static bool _first_gen;
  };

} // namespace axl
