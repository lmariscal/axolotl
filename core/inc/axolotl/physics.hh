#pragma once

#include <axolotl/types.hh>

namespace axl {

  class Scene;

  class RigidBody {
   public:
    bool is_static;
  };

  class Physics {
   public:
    static void Step(Scene &scene, f32 step);
  };

} // namespace axl
