#include <axolotl/ento.hh>
#include <axolotl/geometry.hh>
#include <axolotl/physics.hh>
#include <axolotl/scene.hh>
#include <axolotl/transform.hh>
#include <glm/gtx/matrix_decompose.hpp>

namespace axl {

  constexpr f64 LINEAR_PROJECTION_PERCENT = 0.3;
  constexpr f64 PENETRATION_SLACK = 0.03;
  constexpr i32 IMPULSE_ITERATIONS = 8;

  void RigidBody::Init() { }

  void RigidBody::ApplyForces() {
    Ento ento = Ento::FromComponent(*this);

    m4 parent_model_matrix = ento.Transform().GetParentModelMatrix();
    quat rotation = quat_cast(parent_model_matrix);
    rotation = conjugate(rotation);

    forces = rotation * v3(0.0f, -GRAVITATIONAL_CONSTANT, 0.0f) * (f32)mass;
  }

  f64 RigidBody::InvMass() const {
    return mass == 0.0 ? 0.0 : 1.0 / mass;
  }

  CollisionManifold RigidBody::FindCollisionFeatures(const RigidBody &other) const {
    CollisionManifold result;

    Ento ento = Ento::FromComponent(*this);
    Ento other_ento = Ento::FromComponent(other);

    if (ento.HasComponent<SphereCollider>()) {
      SphereCollider &sphere = ento.GetComponent<SphereCollider>();

      if (other_ento.HasComponent<SphereCollider>()) {
        SphereCollider &other_sphere = other_ento.GetComponent<SphereCollider>();
        result = sphere.SphereCollide(other_sphere);
      } else if (other_ento.HasComponent<OBBCollider>()) {
        OBBCollider &other_obb = other_ento.GetComponent<OBBCollider>();
        result = sphere.OBBCollide(other_obb);
        result.normal = -result.normal;
      }
    } else if (ento.HasComponent<OBBCollider>()) {
      OBBCollider &obb = ento.GetComponent<OBBCollider>();

      if (other_ento.HasComponent<SphereCollider>()) {
        SphereCollider &other_sphere = other_ento.GetComponent<SphereCollider>();
        result = obb.SphereCollide(other_sphere);
        result.normal = -result.normal;
      } else if (other_ento.HasComponent<OBBCollider>()) {
        OBBCollider &other_obb = other_ento.GetComponent<OBBCollider>();
        result = obb.OBBCollide(other_obb);
      }
    }

    return result;
  }

  void RigidBody::ApplyImpulse(RigidBody &other, const CollisionManifold &manifold, i32 c) {
    f64 inv_mass_a = InvMass();
    f64 inv_mass_b = other.InvMass();
    f64 inv_mass = inv_mass_a + inv_mass_b;

    if (inv_mass == 0.0)
      return;

    v3 relative_velocity = other.velocity - velocity;
    v3 relative_normal = normalize(manifold.normal);

    if (dot(relative_velocity, relative_normal) > 0.0)
      return;

    f64 e = min(cor, other.cor);
    f64 numerator = (-(1.0 + e) * dot(relative_velocity, relative_normal));
    f64 j = numerator / inv_mass;

    if (manifold.points.size() > 0.0 && j != 0.0)
      j /= manifold.points.size();

    v3 impulse = (f32)j * relative_normal;
    velocity = velocity - impulse * (f32)inv_mass_a;
    other.velocity = other.velocity + impulse * (f32)inv_mass_b;

    // friction

    v3 t = relative_velocity - (relative_normal * dot(relative_velocity, relative_normal));
    if (epsilonEqual(length2(t), 0.0f, std::numeric_limits<f32>::epsilon()))
      return;
    t = normalize(t);

    numerator = -dot(relative_velocity, t);
    f64 jt = numerator / inv_mass;

    if (manifold.points.size() > 0.0 && jt != 0.0)
      jt /= manifold.points.size();

    if (epsilonEqual(jt, 0.0, std::numeric_limits<f64>::epsilon()))
      return;

    f64 frict = sqrt(friction * other.friction);
    f64 jt_clamped = clamp(jt, -frict, frict);

    v3 tangent_impulse = (f32)jt_clamped * t;

    velocity = velocity - tangent_impulse * (f32)inv_mass_a;
    other.velocity = other.velocity + tangent_impulse * (f32)inv_mass_b;
  }

  void Physics::Step(Scene &scene, f64 step) {
    entt::registry &registry = scene.GetRegistry();

    registry.view<Transform, OBBCollider>().each([&](entt::entity entity, Transform &transform, OBBCollider &collider) {
      if (transform.WasDirty()) {
        v3 skew;
        v4 perspective;

        quat rotation;
        v3 scale;
        v3 translation;

        m4 parent_model_matrix = transform.GetModelMatrix();
        decompose(parent_model_matrix, scale, rotation, translation, skew, perspective);
        rotation = conjugate(rotation);

        collider.position = translation;
        collider.size = scale;
        collider.SetRotation(rotation);
      }
    });
    registry.view<Transform, SphereCollider>().each(
      [&](entt::entity entity, Transform &transform, SphereCollider &collider) {
        if (transform.WasDirty()) {
          v3 skew;
          v4 perspective;
          quat rotation;

          v3 scale;
          v3 translation;

          m4 parent_model_matrix = transform.GetModelMatrix();
          decompose(parent_model_matrix, scale, rotation, translation, skew, perspective);

          collider.position = translation;
          collider.radius = scale.x;
        }
      });

    std::vector<CollisionManifold> manifolds;
    manifolds.reserve(registry.size());
    std::vector<RigidBody *> colliders_a;
    colliders_a.reserve(registry.size());
    std::vector<RigidBody *> colliders_b;
    colliders_b.reserve(registry.size());

    registry.group<Transform, RigidBody>().each([&](entt::entity entity, Transform &transform, RigidBody &body) {
      body.ApplyForces();
      Ento ento = scene.FromHandle(entity);

      v3 acceleration = body.forces * (f32)body.InvMass();
      body.velocity = body.velocity + acceleration * (f32)step;
      body.velocity = body.velocity * (f32)body.damping;

      if (ento.HasAnyOf<SphereCollider, OBBCollider>()) {
        registry.view<Transform, RigidBody>().each(
          [&](entt::entity other_entity, Transform &other_transform, RigidBody &other_body) {
            Ento other_ento = scene.FromHandle(other_entity);

            if (other_ento.id == ento.id)
              return;

            CollisionManifold manifold = body.FindCollisionFeatures(other_body);

            if (!manifold.colliding)
              return;

            manifolds.emplace_back(manifold);
            colliders_a.push_back(&body);
            colliders_b.push_back(&other_body);
          });
      }
    });

    for (i32 i = 0; i < IMPULSE_ITERATIONS; ++i) {
      for (CollisionManifold &manifold : manifolds) {
        i32 p_size = manifold.points.size();
        for (i32 j = 0; j < p_size; ++j) {
          RigidBody *a = colliders_a[j];
          RigidBody *b = colliders_b[j];

          a->ApplyImpulse(*b, manifold, j);
        }
      }
    }

    registry.group<Transform, RigidBody>().each([&](entt::entity entity, Transform &transform, RigidBody &body) {
      if (epsilonNotEqual(length2(body.velocity), 0.0f, std::numeric_limits<f32>::epsilon()))
        transform.SetPosition(transform.GetPosition() + body.velocity * (f32)step);
    });

    for (i32 i = 0; i < manifolds.size(); ++i) {
      CollisionManifold &manifold = manifolds[i];
      RigidBody *a = colliders_a[i];
      RigidBody *b = colliders_b[i];

      f64 total_mass = a->InvMass() + b->InvMass();
      if (total_mass == 0.0)
        continue;

      // if (a->IsTrigger() || b->IsTrigger())
      //   continue;

      f64 depth = max(manifold.depth - PENETRATION_SLACK, 0.0);
      f64 scalar = depth / total_mass;
      v3 correction = manifold.normal * (f32)scalar;

      Ento ento_a = Ento::FromComponent(*a);
      ento_a.Transform().SetPosition(ento_a.Transform().GetPosition() - correction * (f32)a->InvMass());
      Ento ento_b = Ento::FromComponent(*b);
      ento_b.Transform().SetPosition(ento_b.Transform().GetPosition() + correction * (f32)b->InvMass());
    }
  }

} // namespace axl
