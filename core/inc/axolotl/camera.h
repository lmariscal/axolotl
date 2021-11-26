#pragma once

#include <axolotl/types.h>
#include <axolotl/transform.h>
#include <axolotl/component.h>

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

  struct Camera : public Component {
   public:
    Camera();
    ~Camera();

    virtual json Serialize() const;
    virtual void Deserialize(const json &json);
    virtual bool ShowData();
    virtual void Init();

    void MoveCamera(CameraDirection direction, Window &window);
    void RotateCamera(const v2 &mouse_delta, Window &window); // euler angles
    void SetAsActive();
    void SetOrthographic();
    void SetPerspective();
    void SetMovementSpeed(f32 speed);
    void SetMouseSensitivity(f32 sensitivity);
    void UpdateVectors();
    m4 GetViewMatrix();
    m4 GetProjectionMatrix(Window &window);

    static Camera * GetActiveCamera();

   protected:

    inline static Camera * _active_camera;

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
  };

} // namespace axl
