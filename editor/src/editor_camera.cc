#include "editor_camera.hh"

namespace axl {

  EditorCamera::EditorCamera(): _world_up({ 0.0f, 1.0f, 0.0f }), _is_orthographic(false), _fov(60.0f) { }

  EditorCamera::~EditorCamera() { }

  void EditorCamera::UpdateVectors() {
    v3 euler = transform.GetRotationEuler();
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

  m4 EditorCamera::GetProjectionMatrix(Window &window) const {
    v2 window_size = window.GetFrameBufferSize();
    f32 aspect_ratio = window_size.x / window_size.y;
    if (_is_orthographic)
      return ortho(0.0f, window_size.x, 0.0f, window_size.y, 0.1f, 100.0f);
    else
      return perspective(radians(_fov), aspect_ratio, 0.1f, 10000.0f);
  }

  m4 EditorCamera::GetViewMatrix() const {
    return lookAt(transform.GetPosition(), transform.GetPosition() + _front, _up);
  }

} // namespace axl
