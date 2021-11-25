#include <axolotl/transform.h>
#include <axolotl/ento.h>
#include <axolotl/scene.h>
#include <imgui.h>

namespace axl {

  Transform::Transform():
    _position({ 0.0f, 0.0f, 0.0f }),
    _scale({ 1.0f, 1.0f, 1.0f }),
    _rotation()
  { }

  Transform::~Transform() { }

  Transform::Transform(const v3& position, const v3& scale, const quat& rotation):
    _position(position),
    _scale(scale),
    _rotation(rotation)
  { }
  Transform::Transform(const Transform& other):
    _position(other._position),
    _scale(other._scale),
    _rotation(other._rotation)
  { }

  void Transform::Init() { }

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

  json Transform::Serialize() const {
    json j = GetRootNode("transform");
    j["position"] = _position;
    j["scale"] = _scale;
    j["rotation"] = _rotation;
    return j;
  }

  void Transform::Deserialize(const json &j) {
    if (!VerifyRootNode(j, "transform"))
      return;

    if (j.find("position") != j.end())
      _position = j["position"];
    if (j.find("scale") != j.end())
      _scale = j["scale"];
    if (j.find("rotation") != j.end())
      _rotation = j["rotation"];
  }

  bool Transform::ShowData() {
    bool modified = false;

    ImGui::SetNextTreeNodeOpen(true);
    if (ImGui::CollapsingHeader("Transform")) {
      if (axl::ShowData("Position", _position))
        modified = true;
      if (axl::ShowData("Scale", _scale, v3(1.0f)))
        modified = true;
      if (axl::ShowData("Rotation", _rotation))
        modified = true;
    }

    return modified;
  }

  m4 Transform::GetModelMatrix() const {
    m4 model(1.0f);
    model = translate(model, _position);
    model *= toMat4(_rotation);
    model = scale(model, _scale);

    Ento &ento = _scene->GetComponent<Ento>(*_parent);
    if (!ento.parent)
      return model;

    Transform *grandpa = _scene->TryGetComponent<Transform>(ento.parent->entity);
    if (!grandpa)
      return model;

    return grandpa->GetModelMatrix() * model;
  }

} // namespace axl
