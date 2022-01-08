#pragma once

#include <axolotl/component.hh>
#include <axolotl/geometry.hh>
#include <axolotl/types.hh>
#include <vector>

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
    bool is_trigger;
    v3 velocity;
    v3 forces;
    v3 torques;
    v3 orientation;
    v3 angular_velocity;

    inline RigidBody():
      cor(0.5),
      mass(1.0),
      friction(0.6),
      velocity(0.0f),
      forces(0.0f),
      torques(0.0f),
      orientation(0.0f),
      angular_velocity(0.0f),
      is_trigger(false) { }
    inline RigidBody(f64 mass, f64 friction = 0.6, f64 cor = 0.5):
      cor(cor),
      mass(mass),
      friction(friction),
      velocity(0.0f),
      forces(0.0f),
      torques(0.0f),
      orientation(0.0f),
      angular_velocity(0.0f),
      is_trigger(false) { }

    f64 InvMass() const;
    m4 InvTensor() const;
    void Init();
    void Update(f64 step);
    void ApplyForces();
    void AddRotationalImpulse(const v3 &point, const v3 &impulse);
    void AddLinearImpulse(const v3 &impulse);
    void ApplyImpulse(RigidBody &other, const CollisionManifold &manifold, i32 c);
    bool ShowComponent();
    CollisionManifold FindCollisionFeatures(const RigidBody &other) const;

    std::vector<Ento> colliding_with;

    REGISTER_COMPONENT(RigidBody, mass, friction, cor, is_trigger)
  };

} // namespace axl
