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
    bool marked_for_deletion;

    void AddChild(Ento *ento);
    void RemoveChild(Ento *ento);

    Ento *parent;
    entt::entity entity;
    std::vector<Ento *> children;
    std::vector<Component *> components;

    json Serialize() const;

    operator entt::entity() const { return entity; }

   protected:
    static uuids::uuid_random_generator _uuid_generator;
    inline static bool _first_gen = true;
    inline static std::map<uuids::uuid, Ento *> _ento_map;
  };

} // namespace axl
