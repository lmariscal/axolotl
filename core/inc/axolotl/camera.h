#pragma once

#include <axolotl/types.h>
#include <axolotl/transform.h>

namespace axl {

  class Window;

  enum class CameraDirection {
    Up,
    Down,
    Front,
    Back,
    Left,
    Right
  };

  struct Camera {
   public:
    Camera();
    ~Camera();

    void MoveCamera(CameraDirection direction, Window &window);
    void RotateCameraMouse(const vec3 &rotation, Window &window); // euler angles
    void SetAsActive();
    void SetOrthographic();
    void SetPerspective();
    m4 GetViewMatrix();
    m4 GetProjectionMatrix(Window &window);

    static Camera * GetActiveCamera();

   protected:
    void UpdateVectors();

    static Camera * _active_camera;

    Transform _transform;
    v3 _up;
    v3 _front;
    v3 _right;
    v3 _world_up;

    f32 _yaw;
    f32 _pitch;

    f32 _movement_speed;

    bool _is_orthographic;
  };

} // namespace axl
