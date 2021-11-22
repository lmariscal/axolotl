#pragma once

#include <axolotl/types.h>

#include <uuid.h>
#include <array>
#include <random>
#include <vector>

#include <axolotl/component.h>

namespace axl {

  struct Ento {
    Ento();
    ~Ento();

    uuids::uuid id;
    std::string name;

    void AddChild(Ento *ento);
    void RemoveChild(Ento *ento);

    Ento *parent;
    entt::entity entity;
    std::vector<Ento *> children;
    std::vector<Component *> components;

    json Serialize() const;

   protected:
    static std::mt19937 _random_generator;
    static uuids::uuid_random_generator _uuid_generator;
    static bool _first_gen;
  };

} // namespace axl
