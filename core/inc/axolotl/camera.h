#pragma once

#include <axolotl/types.h>
#include <axolotl/transform.h>
#include <axolotl/component.h>
#include <axolotl/ento.h>

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

    json Serialize() const;
    void Deserialize(const json &json);
    bool ShowData(Ento ento);

    void MoveCamera(CameraDirection direction, f32 delta_time);
    void RotateCamera(const v2 &mouse_delta, f32 delta_time); // euler angles
    void SetAsActive(Ento ento);
    void SetOrthographic();
    void SetPerspective();
    void SetMovementSpeed(f32 speed);
    void SetMouseSensitivity(f32 sensitivity);
    void UpdateVectors();
    m4 GetViewMatrix();
    m4 GetProjectionMatrix(Window &window);

    static Camera * GetActiveCamera();

   protected:
    inline static Ento _active_camera_ento;

    Transform transform;

    v3 _last_position;
    v3 _last_rotation;

    v3 _up;
    v3 _front;
    v3 _right;
    v3 _world_up;

    f32 _movement_speed;
    f32 _mouse_sensitivity;
    f32 _fov;

    bool _is_orthographic;
    bool _is_active_camera;
  };

} // namespace axl
