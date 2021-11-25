#include <axolotl/camera.h>

#include <axolotl/window.h>
#include <axolotl/iomanager.h>
#include <axolotl/scene.h>
#include <algorithm>

namespace axl {

  Camera::Camera():
    _is_orthographic(false),
    _world_up({ 0.0f, 1.0f, 0.0f }),
    _movement_speed(6.0f),
    _mouse_sensitivity(2.0f)
  {
  }

  Camera::~Camera() {
  }

  void Camera::Init() {
    Transform &transform = _scene->TryAddComponent<Transform>(*_parent);
    transform.SetPosition({ 0.0f, 0.0f, 0.0f });
    transform.SetRotation(v3(90.0f, 0.0f, 0.0f));
    UpdateVectors();
  }

  void Camera::MoveCamera(CameraDirection direction, Window &window) {
    if (_active_camera != this)
      return;

    Transform &transform = _scene->GetComponent<Transform>(*_parent);
    IOManager *io_manager = window.GetIOManager();
    f32 delta = window.GetDeltaTime();
    switch (direction) {
      case CameraDirection::Down:
        transform.SetPosition(transform.GetPosition() + _up * _movement_speed * delta);
        break;
      case CameraDirection::Up:
        transform.SetPosition(transform.GetPosition() - _up * _movement_speed * delta);
        break;
      case CameraDirection::Front:
        transform.SetPosition(transform.GetPosition() + _front * _movement_speed * delta);
        break;
      case CameraDirection::Back:
        transform.SetPosition(transform.GetPosition() - _front * _movement_speed * delta);
        break;
      case CameraDirection::Left:
        transform.SetPosition(transform.GetPosition() - _right * _movement_speed * delta);
        break;
      case CameraDirection::Right:
        transform.SetPosition(transform.GetPosition() + _right * _movement_speed * delta);
        break;
    }
  }

  void Camera::RotateCamera(const v2 &mouse_delta, Window &window) {
    if (_active_camera != this)
      return;

    IOManager *io_manager = window.GetIOManager();
    f32 delta = window.GetDeltaTime();
    Transform &transform = _scene->GetComponent<Transform>(*_parent);

    v3 euler = transform.GetRotation();
    f32 &yaw = euler.x;
    f32 &pitch = euler.y;

    yaw += mouse_delta.x * delta * _mouse_sensitivity * 10.0f;
    pitch -= mouse_delta.y * delta * _mouse_sensitivity * 10.0f;

    transform.SetRotation(euler);

    UpdateVectors();
  }

  m4 Camera::GetProjectionMatrix(Window &window) {
    v2 window_size = window.GetFrameBufferSize();
    f32 aspect_ratio = window_size.x / window_size.y;
    if (_is_orthographic)
      return ortho(0.0f, window_size.x, 0.0f, window_size.y, 0.1f, 100.0f);
    else
      return perspective(radians(45.0f), aspect_ratio, 0.1f, 100.0f);
  }

  void Camera::UpdateVectors() {
    Transform &transform = _scene->GetComponent<Transform>(*_parent);
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

  m4 Camera::GetViewMatrix() {
    Transform &transform = _scene->GetComponent<Transform>(*_parent);
    return lookAt(transform.GetPosition(), transform.GetPosition() + _front, _up);
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

  json Camera::Serialize() const {
    json j = GetRootNode("camera");
    j["is_orthographic"] = _is_orthographic;
    j["movement_speed"] = _movement_speed;
    j["mouse_sensitivity"] = _mouse_sensitivity;
    return j;
  }

  void Camera::Deserialize(const json &j) {
    if (!VerifyRootNode(j, "transform"))
      return;

    if (j.find("is_orthographic") != j.end())
      _is_orthographic = j["is_orthographic"];
    if (j.find("movement_speed") != j.end())
      _movement_speed = j["movement_speed"];
    if (j.find("mouse_sensitivity") != j.end())
      _mouse_sensitivity = j["mouse_sensitivity"];
  }

  bool Camera::ShowData() {
    bool modified = false;

    ImGui::SetNextTreeNodeOpen(true);
    if (ImGui::CollapsingHeader("Camera")) {
      if (axl::ShowData("Orthographic", _is_orthographic))
        modified = true;
      if (axl::ShowData("Movement Speed", _movement_speed))
        modified = true;
      if (axl::ShowData("Mouse Sensitivity", _mouse_sensitivity))
        modified = true;
    }

    Transform &transform = _scene->GetComponent<Transform>(*_parent);
    if (_last_position != transform.GetPosition()) {
      _last_position = transform.GetPosition();
      modified = true;
    }
    if (_last_rotation != transform.GetRotation()) {
      _last_rotation = transform.GetRotation();
      modified = true;
    }

    if (modified)
      UpdateVectors();

    return modified;
  }

} // namespace axl
