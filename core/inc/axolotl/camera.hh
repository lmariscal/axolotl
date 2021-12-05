#pragma once

#include <axolotl/types.hh>
#include <axolotl/transform.hh>
#include <axolotl/component.hh>
#include <axolotl/ento.hh>

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
    Camera(Ento &ento);
    ~Camera();

    json Serialize() const;
    void Deserialize(const json &json);
    bool ShowComponent(Ento &ento);

    void MoveCamera(Ento &ento, CameraDirection direction, f64 delta);
    void RotateCamera(Ento &ento, const v2 &mouse_delta, f64 delta); // euler angles
    void SetAsActive(Ento ento);
    void SetOrthographic();
    void SetPerspective();
    void SetMovementSpeed(f32 speed);
    void SetMouseSensitivity(f32 sensitivity);
    void UpdateVectors(Ento &ento);
    m4 GetViewMatrix(Ento &ento);
    m4 GetProjectionMatrix(Window &window);

    static Camera * GetActiveCamera();
    static Ento GetActiveCameraEnto();

   protected:
    inline static Ento _active_camera_ento;

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
