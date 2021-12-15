#pragma once

#include <axolotl/transform.hh>
#include <axolotl/types.hh>
#include <axolotl/window.hh>

namespace axl {

  // TODO: Create Camera hierarchy and inherit from it.
  class EditorCamera {
   public:
    EditorCamera();
    ~EditorCamera();

    void UpdateVectors();
    m4 GetViewMatrix() const;
    m4 GetProjectionMatrix(Window &window) const;

    void SetFov(f32 fov);

    Transform transform;

   protected:
    v3 _up;
    v3 _front;
    v3 _right;
    v3 _world_up;

    bool _is_orthographic;
    f32 _fov;
  };

} // namespace axl
