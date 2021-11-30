#pragma once

#include <axolotl/types.h>

#include <uuid.h>
#include <array>
#include <random>
#include <vector>

#include <axolotl/component.h>

namespace axl {

  struct Ento {
    Ento(entt::entity e, Scene &scene);
    ~Ento();

    void AddChild(entt::entity e);
    void RemoveChild(entt::entity e);
    json Serialize() const;

    uuids::uuid id;
    std::string name;
    bool marked_for_deletion;

    entt::entity parent;
    entt::entity entity;
    std::vector<Component *> components; // These are refs, we do not own them
    std::vector<entt::entity> children;

    Scene *scene;

   protected:
    static entt::entity FromID(uuids::uuid id);

    static uuids::uuid_random_generator _uuid_generator;
    inline static bool _first_gen = true;
    inline static std::map<uuids::uuid, entt::entity> _ento_map;
  };

} // namespace axl
