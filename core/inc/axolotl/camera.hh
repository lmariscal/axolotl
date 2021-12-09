#pragma once

#include <axolotl/component.hh>
#include <axolotl/ento.hh>
#include <axolotl/transform.hh>
#include <axolotl/types.hh>

namespace axl {

  class Window;

  enum class CameraDirection { Up, Down, Front, Back, Left, Right };

  struct Camera {
   public:
    Camera();
    ~Camera();

    json Serialize() const;
    void Deserialize(const json &json);
    bool ShowComponent(Ento &ento);

    void Init();
    void MoveCamera(CameraDirection direction, f64 delta);
    void RotateCamera(const v2 &mouse_delta, f64 delta); // euler angles
    void SetAsActive();
    void SetOrthographic();
    void SetPerspective();
    void SetMovementSpeed(f32 speed);
    void SetMouseSensitivity(f32 sensitivity);
    void UpdateVectors();
    m4 GetViewMatrix();
    m4 GetProjectionMatrix(Window &window);

    static Camera *GetActiveCamera();
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
