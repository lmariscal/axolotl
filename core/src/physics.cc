#include <axolotl/physics.hh>
#include <axolotl/scene.hh>
#include <axolotl/transform.hh>

namespace axl {

  void Physics::Step(Scene &scene, f32 step) {
    entt::registry &registry = scene.GetRegistry();

    registry.group<Transform, RigidBody>().each([&](entt::entity entity, Transform &transform, RigidBody &body) {
      if (body.is_static)
        return;
    });
  }

} // namespace axl
