#pragma once

#include <axolotl/component.hh>
#include <axolotl/geometry.hh>
#include <axolotl/types.hh>

namespace axl {

  class Scene;

  class Physics {
   public:
    static void Step(Scene &scene, f64 step);
  };

  class RigidBody {
   public:
    inline static const f64 damping = 0.98;

    f64 mass;
    f64 friction;
    f64 cor;
    v3 velocity;
    v3 forces;

    inline RigidBody(): cor(0.5), mass(1.0), friction(0.6), velocity(0.0f), forces(0.0f) { }
    inline RigidBody(f64 mass, f64 friction = 0.6, f64 cor = 0.5):
      cor(cor),
      mass(mass),
      friction(friction),
      velocity(0.0f),
      forces(0.0f) { }

    f64 InvMass() const;
    void Init();
    void ApplyForces();
    CollisionManifold FindCollisionFeatures(const RigidBody &other) const;
    void ApplyImpulse(RigidBody &other, const CollisionManifold &manifold, i32 c);

    REGISTER_COMPONENT(RigidBody, mass, friction, cor)
  };

} // namespace axl
