#include <axolotl/camera.hh>

#include <axolotl/window.hh>
#include <axolotl/iomanager.hh>
#include <axolotl/scene.hh>
#include <axolotl/ento.hh>
#include <algorithm>

namespace axl {

  Camera::Camera(Ento &ento):
    _is_orthographic(false),
    _world_up({ 0.0f, 1.0f, 0.0f }),
    _movement_speed(30.0f),
    _mouse_sensitivity(2.0f),
    _fov(60.0f),
    _is_active_camera(false)
  {
    Transform &transform = ento.Transform();
    transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    transform.SetRotation(v3(90.0f, 0.0f, 0.0f));
    UpdateVectors(ento);
  }

  Camera::~Camera() {
    if (_is_active_camera)
      _active_camera_ento = { };
  }

  void Camera::MoveCamera(Ento &ento, CameraDirection direction, f64 delta) {
    Transform &transform = ento.Transform();
    switch (direction) {
      case CameraDirection::Down:
        transform.SetPosition(transform.GetPosition() + _world_up * _movement_speed * (f32)delta);
        break;
      case CameraDirection::Up:
        transform.SetPosition(transform.GetPosition() - _world_up * _movement_speed * (f32)delta);
        break;
      case CameraDirection::Front:
        transform.SetPosition(transform.GetPosition() + _front * _movement_speed * (f32)delta);
        break;
      case CameraDirection::Back:
        transform.SetPosition(transform.GetPosition() - _front * _movement_speed * (f32)delta);
        break;
      case CameraDirection::Left:
        transform.SetPosition(transform.GetPosition() - _right * _movement_speed * (f32)delta);
        break;
      case CameraDirection::Right:
        transform.SetPosition(transform.GetPosition() + _right * _movement_speed * (f32)delta);
        break;
    }
  }

  void Camera::RotateCamera(Ento &ento, const v2 &mouse_delta, f64 delta) {
    Transform &transform = ento.Transform();
    v3 euler = transform.GetRotation();
    f32 &yaw = euler.x;
    f32 &pitch = euler.y;

    yaw += mouse_delta.x * (f32)delta * _mouse_sensitivity * 10.0f;
    pitch -= mouse_delta.y * (f32)delta * _mouse_sensitivity * 10.0f;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    std::min(yaw, 360.0f);
    std::max(yaw, 0.0f);

    transform.SetRotation(euler);

    UpdateVectors(ento);
  }

  m4 Camera::GetProjectionMatrix(Window &window) {
    v2 window_size = window.GetFrameBufferSize();
    f32 aspect_ratio = window_size.x / window_size.y;
    if (_is_orthographic)
      return ortho(0.0f, window_size.x, 0.0f, window_size.y, 0.1f, 100.0f);
    else
      return perspective(radians(_fov), aspect_ratio, 0.1f, 10000.0f);
  }

  void Camera::UpdateVectors(Ento &ento) {
    Transform &transform = ento.Transform();
    v3 euler = transform.GetRotation();
    f32 &yaw = euler.x;
    f32 &pitch = euler.y;

    v3 front;
    front.x = cos(radians(yaw)) * cos(radians(pitch));
    front.y = sin(radians(pitch));
    front.z = sin(radians(yaw)) * cos(radians(pitch));
    _front = normalize(front);
    _right = normalize(cross(_front, _world_up));
    _up = normalize(cross(_right, _front));
  }

  m4 Camera::GetViewMatrix(Ento &ento) {
    Transform &transform = ento.Transform();
    return lookAt(transform.GetPosition(), transform.GetPosition() + _front, _up);
  }

  void Camera::SetAsActive(Ento ento) {
    if (_active_camera_ento)
      _active_camera_ento.GetComponent<Camera>()._is_active_camera = false;
    _active_camera_ento = ento;
    _is_active_camera = true;
  }

  Camera * Camera::GetActiveCamera() {
    if (!_active_camera_ento)
      return nullptr;
    return &_active_camera_ento.GetComponent<Camera>();
  }

  Ento Camera::GetActiveCameraEnto() {
    return _active_camera_ento;
  }

  void Camera::SetPerspective() {
    _is_orthographic = false;
  }

  void Camera::SetOrthographic() {
    _is_orthographic = true;
  }

  json Camera::Serialize() const {
    json j;
    j["is_orthographic"] = _is_orthographic;
    j["movement_speed"] = _movement_speed;
    j["mouse_sensitivity"] = _mouse_sensitivity;
    return j;
  }

  void Camera::Deserialize(const json &j) {
    if (j.find("is_orthographic") != j.end())
      _is_orthographic = j["is_orthographic"];
    if (j.find("movement_speed") != j.end())
      _movement_speed = j["movement_speed"];
    if (j.find("mouse_sensitivity") != j.end())
      _mouse_sensitivity = j["mouse_sensitivity"];
  }

  bool Camera::ShowComponent(Ento &ento) {
    bool modified = false;

    bool active = _is_active_camera;
    if (ShowData("Active", active)) {
      if (active) {
        SetAsActive(ento);
      } else {
        if (GetActiveCameraEnto() == ento)
          SetAsActive({ });
        _is_active_camera = false;
      }
      modified = true;
    }
    if (ShowData("Orthographic", _is_orthographic))
      modified = true;
    if (ShowData("Speed", _movement_speed))
      modified = true;
    if (ShowData("Sensitivity", _mouse_sensitivity))
      modified = true;
    if (ShowData("FOV", _fov))
      modified = true;

    return modified;
  }

} // namespace axl
