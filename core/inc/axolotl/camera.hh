#pragma once

#include <axolotl/component.hh>
#include <axolotl/ento.hh>
#include <axolotl/transform.hh>
#include <axolotl/types.hh>

namespace axl {

  class Window;

  enum class CameraDirection { Up, Down, Front, Back, Left, Right };

  class Camera {
   public:
    Camera();
    ~Camera();

    bool ShowComponent(Ento &ento);

    void Init();
    void MoveCamera(CameraDirection direction, f64 delta);
    void RotateCamera(const v2 &mouse_delta, f64 delta); // euler angles
    void SetAsActive();
    void SetOrthographic();
    void SetPerspective();
    void SetMovementSpeed(f32 speed);
    void SetMouseSensitivity(f32 sensitivity);
    void SetFov(f32 fov);
    void UpdateVectors(Transform *transform);
    void SetCustomViewMatrix(m4 view);
    v3 GetFront();
    v3 GetUp();
    v3 GetRight();
    m4 GetViewMatrix(Transform *transform);
    m4 GetProjectionMatrix(Window &window);

    static void SetAsActive(Camera *camera);
    static Camera *GetActiveCamera();
    static Ento GetActiveCameraEnto();

    REGISTER_COMPONENT(Camera, _movement_speed, _mouse_sensitivity, _fov, _is_orthographic, _is_active_camera)

   protected:
    inline static Ento _active_camera_ento;

    v3 _up;
    v3 _front;
    v3 _right;
    v3 _world_up;

    f32 _movement_speed;
    f32 _mouse_sensitivity;
    f32 _fov;

    m4 _view_matrix;

    bool _is_orthographic;
    bool _is_active_camera;
  };

} // namespace axl
