#include <axolotl/camera.h>

#include <axolotl/window.h>
#include <axolotl/iomanager.h>
#include <algorithm>

namespace axl {

  Camera *Camera::_active_camera = nullptr;

  Camera::Camera():
    _is_orthographic(true),
    _world_up({ 0.0f, 1.0f, 0.0f }),
    _yaw(-90.0f),
    _pitch(0.0f),
    _movement_speed(6.0f)
  {
    _transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    UpdateVectors();
  }

  Camera::~Camera() {
  }

  void Camera::MoveCamera(CameraDirection direction, Window &window) {
    if (_active_camera != this)
      return;

    IOManager *io_manager = window.GetIOManager();
    f32 delta = window.GetDeltaTime() / 1000.0f;
    switch (direction) {
      case CameraDirection::Up:
        _transform.SetPosition(_transform.GetPosition() + _up * _movement_speed * delta);
        break;
      case CameraDirection::Down:
        _transform.SetPosition(_transform.GetPosition() - _up * _movement_speed * delta);
        break;
      case CameraDirection::Front:
        _transform.SetPosition(_transform.GetPosition() + _front * _movement_speed * delta);
        break;
      case CameraDirection::Back:
        _transform.SetPosition(_transform.GetPosition() - _front * _movement_speed * delta);
        break;
      case CameraDirection::Left:
        _transform.SetPosition(_transform.GetPosition() - _right * _movement_speed * delta);
        break;
      case CameraDirection::Right:
        _transform.SetPosition(_transform.GetPosition() + _right * _movement_speed * delta);
        break;
    }
  }

  void Camera::RotateCameraMouse(const vec3 &rotation, Window &window) {
    if (_active_camera != this)
      return;

    IOManager *io_manager = window.GetIOManager();
    f32 delta = window.GetDeltaTime() / 1000.0f;
    _yaw += rotation.x * delta;
    _pitch -= rotation.y * delta;

    _pitch = std::min(_pitch, 90.0f);
    _pitch = std::max(_pitch, -90.0f);

    if (_yaw > 360.0f)
      _yaw -= 360.0f;
    else if (_yaw < 0.0f)
      _yaw += 360.0f;

    UpdateVectors();
  }

  m4 Camera::GetProjectionMatrix(Window &window) {
    v2 window_size = window.GetFramebufferSize();
    f32 aspect_ratio = window_size.x / window_size.y;
    if (_is_orthographic)
      return ortho(0.0f, window_size.x, 0.0f, window_size.y, 0.1f, 100.0f);
    else
      return perspective(radians(45.0f), aspect_ratio, 0.1f, 100.0f);
  }

  void Camera::UpdateVectors() {
    v3 front;
    front.x = cos(radians(_yaw)) * cos(radians(_pitch));
    front.y = sin(radians(_pitch));
    front.z = sin(radians(_yaw)) * cos(radians(_pitch));
    _front = normalize(front);
    _right = normalize(cross(_front, _world_up));
    _up = normalize(cross(_right, _front));
  }

  m4 Camera::GetViewMatrix() {
    return lookAt(_transform.GetPosition(), _transform.GetPosition() + _front, _up);
  }

  void Camera::SetAsActive() {
    _active_camera = this;
  }

  Camera * Camera::GetActiveCamera() {
    return _active_camera;
  }

  void Camera::SetPerspective() {
    _is_orthographic = false;
  }

  void Camera::SetOrthographic() {
    _is_orthographic = true;
  }

} // namespace axl
