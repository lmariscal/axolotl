#include <axolotl/transform.hh>
#include <axolotl/ento.hh>
#include <axolotl/scene.hh>
#include <imgui.h>

namespace axl {

  Transform::Transform():
    _position({ 0.0f, 0.0f, 0.0f }),
    _scale({ 1.0f, 1.0f, 1.0f }),
    _rotation()
  { }

  Transform::Transform(const v3& position, const v3& scale, const quat& rotation):
    _position(position),
    _scale(scale),
    _rotation(rotation)
  { }

  const v3 & Transform::GetPosition() const {
    return _position;
  }

  const v3 & Transform::GetScale() const {
    return _scale;
  }

  const quat & Transform::GetRotationQuat() const {
    return _rotation;
  }

  const v3 Transform::GetRotation() const {
    return degrees(eulerAngles(_rotation));
  }

  void Transform::SetRotation(const v3 &rotation) {
    _rotation = quat(radians(rotation));
  }

  void Transform::SetPosition(const v3& position) {
    _position = position;
  }

  void Transform::SetScale(const v3& scale) {
    _scale = scale;
  }

  void Transform::SetRotation(const quat& rotation) {
    _rotation = rotation;
  }

  m4 Transform::GetModelMatrix(Ento ento) const {
    m4 model(1.0f);
    model = translate(model, _position);
    model *= toMat4(_rotation);
    model = scale(model, _scale);

    if (!ento.HasParent() || !ento.Parent())
      return model;

    Ento parent = ento.Parent();
    return parent.GetComponent<Transform>().GetModelMatrix(parent) * model;
  }

  bool Transform::ShowComponent() {
    bool modified = false;

    if (ShowData("Position", _position))
      modified = true;
    if (ShowData("Scale", _scale, v3(1.0f)))
      modified = true;
    if (ShowData("Rotation", _rotation))
      modified = true;

    return modified;
  }

  json Transform::Serialize() const {
    json j;
    j["version"]["major"] = 0;
    j["version"]["minor"] = 1;
    j["type"] = "transform";

    j["position"] = _position;
    j["scale"] = _scale;
    j["rotation"] = _rotation;
    return j;
  }

  void Transform::Deserialize(const json &json) {
  }

} // namespace axl
