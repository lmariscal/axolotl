#include <axolotl/ento.hh>
#include <axolotl/geometry.hh>
#include <axolotl/physics.hh>
#include <axolotl/scene.hh>
#include <axolotl/transform.hh>
#include <axolotl/window.hh>
#include <glm/gtx/matrix_decompose.hpp>

namespace axl {

  constexpr f64 LINEAR_PROJECTION_PERCENT = 0.6;
  constexpr f64 PENETRATION_SLACK = 0.01;
  constexpr i32 IMPULSE_ITERATIONS = 9;

  void RigidBody::Init() { }

  void RigidBody::ApplyForces() {
    Ento ento = Ento::FromComponent(*this);

    m4 parent_model_matrix(1.0f);
    if (ento.HasParent())
      parent_model_matrix = ento.Transform().GetParentModelMatrix();
    quat rotation = quat_cast(parent_model_matrix);
    rotation = conjugate(rotation);

    forces = rotation * v3(0.0f, -GRAVITATIONAL_CONSTANT, 0.0f) * (f32)mass;
  }

  f64 RigidBody::InvMass() const {
    return mass == 0.0 ? 0.0 : 1.0 / mass;
  }

  m4 RigidBody::InvTensor() const {
    if (mass == 0.0)
      return m4(0.0);

    Ento ento = Ento::FromComponent(*this);

    f64 ix = 0.0;
    f64 iy = 0.0;
    f64 iz = 0.0;
    f64 iw = 0.0;

    if (ento.HasComponent<SphereCollider>()) {
      SphereCollider sphere = ento.GetComponent<SphereCollider>();
      f64 r2 = sphere.radius * sphere.radius;
      constexpr f64 fraction = (2.0 / 5.0);

      ix = r2 * mass * fraction;
      iy = r2 * mass * fraction;
      iz = r2 * mass * fraction;
      iw = 1.0;
    } else if (ento.HasComponent<OBBCollider>()) {
      OBBCollider obb = ento.GetComponent<OBBCollider>();
      v3 size = obb.size * 2.0f;
      constexpr f64 fraction = (1.0 / 12.0);
      f64 x2 = size.x * size.x;
      f64 y2 = size.y * size.y;
      f64 z2 = size.z * size.z;

      ix = (y2 + z2) * mass * fraction;
      iy = (x2 + z2) * mass * fraction;
      iz = (x2 + y2) * mass * fraction;
      iw = 1.0;
    }

    return inverse(m4((f32)ix, 0, 0, 0, 0, (f32)iy, 0, 0, 0, 0, (f32)iz, 0, 0, 0, 0, (f32)iw));
  }

  void RigidBody::AddRotationalImpulse(const v3 &point, const v3 &impulse) {
    Ento ento = Ento::FromComponent(*this);

    v3 center_of_mass = ento.Transform().GetPosition();
    v3 torque = cross(point - center_of_mass, impulse);
    v3 angular_acceleration = v4(torque, 1.0) * InvTensor();
    angular_velocity = angular_velocity + angular_acceleration;
  }

  void RigidBody::AddLinearImpulse(const v3 &impulse) {
    velocity = velocity + impulse;
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
      // INFO: This works, just that performance is terrible, and it is not used in the game.
      //
      // } else if (ento.HasComponent<OBBCollider>()) {
      //   OBBCollider &obb = ento.GetComponent<OBBCollider>();

      //   if (other_ento.HasComponent<SphereCollider>()) {
      //     SphereCollider &other_sphere = other_ento.GetComponent<SphereCollider>();
      //     result = obb.SphereCollide(other_sphere);
      //   } else if (other_ento.HasComponent<OBBCollider>()) {
      //     OBBCollider &other_obb = other_ento.GetComponent<OBBCollider>();
      //     result = obb.OBBCollide(other_obb);
      //     result.normal = -result.normal;
      //   }
    }

    return result;
  }

  void RigidBody::ApplyImpulse(RigidBody &other, const CollisionManifold &manifold, i32 c) {
    f64 inv_mass_a = InvMass();
    f64 inv_mass_b = other.InvMass();
    f64 inv_mass = inv_mass_a + inv_mass_b;

    if (inv_mass == 0.0)
      return;

    Ento ento = Ento::FromComponent(*this);
    Ento other_ento = Ento::FromComponent(other);

    v3 relative_point = manifold.points[c];
    v3 r_a = relative_point - ento.Transform().GetPosition();
    v3 r_b = relative_point - other_ento.Transform().GetPosition();
    m4 i_a = InvTensor();
    m4 i_b = other.InvTensor();

    v3 relative_velocity =
      (other.velocity + cross(other.angular_velocity, r_b)) - (velocity + cross(angular_velocity, r_a));
    v3 relative_normal = normalize(manifold.normal);

    if (dot(relative_velocity, relative_normal) > 0.0)
      return;

    f64 e = min(cor, other.cor);
    f64 numerator = (-(1.0 + e) * dot(relative_velocity, relative_normal));
    f64 d1 = inv_mass;
    v3 d2 = cross(v3(v4(cross(r_a, relative_normal), 1.0) * i_a), r_a);
    v3 d3 = cross(v3(v4(cross(r_b, relative_normal), 1.0) * i_b), r_b);
    f64 denominator = d1 + dot(relative_normal, d2 + d3);
    f64 j = (denominator == 0.0) ? 0.0 : numerator / denominator;

    if (manifold.points.size() > 0.0 && j != 0.0)
      j /= manifold.points.size();

    v3 impulse = (f32)j * relative_normal;
    velocity = velocity - impulse * (f32)inv_mass_a;
    other.velocity = other.velocity + impulse * (f32)inv_mass_b;

    angular_velocity = angular_velocity - v3(v4(cross(r_a, impulse), 1.0) * i_a);
    other.angular_velocity = other.angular_velocity + v3(v4(cross(r_b, impulse), 1.0) * i_b);

    // friction

    v3 t = relative_velocity - (relative_normal * dot(relative_velocity, relative_normal));
    if (epsilonEqual(length2(t), 0.0f, std::numeric_limits<f32>::epsilon()))
      return;
    t = normalize(t);

    numerator = -dot(relative_velocity, t);
    d1 = inv_mass;
    d2 = cross(v3(v4(cross(r_a, t), 1.0) * i_a), r_a);
    d3 = cross(v3(v4(cross(r_b, t), 1.0) * i_b), r_b);
    denominator = d1 + dot(t, d2 + d3);

    f64 jt = denominator == 0.0 ? 0.0f : numerator / denominator;

    if (manifold.points.size() > 0.0 && jt != 0.0)
      jt /= manifold.points.size();

    if (epsilonEqual(jt, 0.0, std::numeric_limits<f64>::epsilon()))
      return;

    f64 frict = sqrt(friction * other.friction);
    f64 jt_clamped = clamp(jt, -frict, frict);

    v3 tangent_impulse = (f32)jt_clamped * t;

    velocity = velocity - tangent_impulse * (f32)inv_mass_a;
    other.velocity = other.velocity + tangent_impulse * (f32)inv_mass_b;
    angular_velocity = angular_velocity - v3(v4(cross(r_a, tangent_impulse), 1.0) * i_a);
    other.angular_velocity = other.angular_velocity + v3(v4(cross(r_b, tangent_impulse), 1.0) * i_b);
  }

  void RigidBody::Update(f64 step) {
    colliding_with.clear();
    Ento ento = Ento::FromComponent(*this);
    v3 acceleration = forces * (f32)InvMass();
    velocity = velocity + acceleration * (f32)step;
    velocity = velocity * (f32)damping;

    if (abs(velocity.x) < 0.001)
      velocity.x = 0.0f;
    if (abs(velocity.y) < 0.001)
      velocity.y = 0.0f;
    if (abs(velocity.z) < 0.001)
      velocity.z = 0.0f;

    if (ento.HasComponent<OBBCollider>()) {
      v3 angular_acceleration = v4(torques, 1.0f) * InvTensor();
      angular_velocity = angular_velocity + angular_acceleration * (f32)step;
      angular_velocity = angular_velocity * (f32)damping;

      if (angular_velocity.x < 0.001)
        angular_velocity.x = 0.0f;
      if (angular_velocity.y < 0.001)
        angular_velocity.y = 0.0f;
      if (angular_velocity.z < 0.001)
        angular_velocity.z = 0.0f;
    }

    // ento.Transform().SetPosition(ento.Transform().GetPosition() + velocity * (f32)step);
    // ento.Transform().SetRotationEuler(ento.Transform().GetRotationEuler() + angular_velocity * (f32)step);
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
    i32 rb_times = 0;

    std::vector<CollisionManifold> manifolds;
    manifolds.reserve(registry.size());
    std::vector<RigidBody *> colliders_a;
    colliders_a.reserve(registry.size());
    std::vector<RigidBody *> colliders_b;
    colliders_b.reserve(registry.size());

    registry.group<Transform, RigidBody>().each([&](entt::entity entity, Transform &transform, RigidBody &body) {
      body.ApplyForces();
      body.Update(step);
      Ento ento = scene.FromHandle(entity);

      if (ento.HasAnyOf<SphereCollider, OBBCollider>()) {
        registry.view<Transform, RigidBody>().each(
          [&](entt::entity other_entity, Transform &other_transform, RigidBody &other_body) {
            Ento other_ento = scene.FromHandle(other_entity);

            if (other_ento.id == ento.id)
              return;

            f64 start = Window::GetCurrentWindow()->GetTime();
            CollisionManifold manifold = body.FindCollisionFeatures(other_body);
            f64 end = Window::GetCurrentWindow()->GetTime();
            total_physics_time += end - start;

            if (!manifold.colliding)
              return;

            body.colliding_with.push_back(other_ento);

            if (body.is_trigger || other_body.is_trigger)
              return;

            manifolds.emplace_back(manifold);
            colliders_a.push_back(&body);
            colliders_b.push_back(&other_body);
          });
      }
    });
    // log::info("Physics Step: {}", rb_times);
    // total_physics_time /= (f32)rb_times;

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
      Ento ento = scene.FromHandle(entity);
      if (epsilonNotEqual(length2(body.velocity), 0.0f, std::numeric_limits<f32>::epsilon())) {
        transform.SetPosition(transform.GetPosition() + body.velocity * (f32)step);
      }
      if (ento.HasComponent<OBBCollider>() &&
          epsilonNotEqual(length2(body.angular_velocity), 0.0f, std::numeric_limits<f32>::epsilon())) {
        transform.SetRotationEuler(transform.GetRotationEuler() + degrees(body.angular_velocity) * (f32)step);
        // log::debug("Ento {} has angular velocity {}, step is {}",
        //            ento.Tag().value,
        //            to_string(body.angular_velocity),
        //            step);
      }
    });

    for (i32 i = 0; i < manifolds.size(); ++i) {
      CollisionManifold &manifold = manifolds[i];
      RigidBody *a = colliders_a[i];
      RigidBody *b = colliders_b[i];

      f64 total_mass = a->InvMass() + b->InvMass();
      if (total_mass == 0.0)
        continue;

      f64 depth = max(manifold.depth - PENETRATION_SLACK, 0.0);
      f64 scalar = depth / total_mass;
      v3 correction = manifold.normal * (f32)scalar * (f32)LINEAR_PROJECTION_PERCENT;

      Ento ento_a = Ento::FromComponent(*a);
      ento_a.Transform().SetPosition(ento_a.Transform().GetPosition() - correction * (f32)a->InvMass());
      Ento ento_b = Ento::FromComponent(*b);
      ento_b.Transform().SetPosition(ento_b.Transform().GetPosition() + correction * (f32)b->InvMass());
    }
  }

  bool RigidBody::ShowComponent() {
    bool modified = false;

    if (!ShowData("Mass", mass))
      modified = true;
    if (!ShowData("Friction", friction))
      modified = true;
    if (!ShowData("Restitution", cor))
      modified = true;
    if (!ShowData("Is Trigger", is_trigger))
      modified = true;

    return modified;
  }

} // namespace axl
