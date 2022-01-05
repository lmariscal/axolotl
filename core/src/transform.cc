#include <axolotl/component.hh>
#include <axolotl/ento.hh>
#include <axolotl/scene.hh>
#include <axolotl/transform.hh>
#include <imgui.h>

namespace axl {

  Transform::Transform(): _position({ 0.0f, 0.0f, 0.0f }), _scale({ 1.0f, 1.0f, 1.0f }), _rotation() { }

  Transform::Transform(const v3 &position, const v3 &scale, const quat &rotation):
    _position(position),
    _scale(scale),
    _rotation(rotation),
    _is_dirty(true),
    _model_matrix(1.0f),
    _parent_model_matrix(1.0f) { }

  void Transform::Init() {
    _is_dirty = true;
  }

  const v3 &Transform::GetPosition() const {
    return _position;
  }

  const v3 &Transform::GetScale() const {
    return _scale;
  }

  const quat &Transform::GetRotation() const {
    return _rotation;
  }

  const v3 Transform::GetRotationEuler() const {
    return degrees(eulerAngles(_rotation));
  }

  void Transform::SetRotationEuler(const v3 &rotation) {
    _rotation = quat(radians(rotation));
    _is_dirty = true;
  }

  void Transform::SetPosition(const v3 &position) {
    _position = position;
    _is_dirty = true;
  }

  void Transform::SetScale(const v3 &scale) {
    _scale = scale;
    _is_dirty = true;
  }

  void Transform::SetRotation(const quat &rotation) {
    _rotation = rotation;
    _is_dirty = true;
  }

  bool Transform::IsDirty() const {
    if (_is_dirty)
      return true;

    Ento ento = Ento::FromComponent(*this);
    if (!ento.HasParent())
      return _is_dirty;

    return ento.Parent().Transform()._model_matrix != _parent_model_matrix;
  }

  m4 Transform::GetModelMatrix() {
    Ento ento = Ento::FromComponent(*this);

    if (!IsDirty()) {
      if (!ento.HasParent())
        return _model_matrix;

      if (!ento.Parent().Transform().IsDirty())
        return _model_matrix;
    }

    m4 model(1.0f);
    model = translate(model, _position);
    model *= toMat4(_rotation);
    model = scale(model, _scale);

    if (ento.HasParent() && ento.Parent()) {
      Ento parent = ento.Parent();
      _parent_model_matrix = parent.Transform().GetModelMatrix();
      model = _parent_model_matrix * model;
    }

    _model_matrix = model;
    _is_dirty = false;
    _was_dirty = true;
    return model;
  }

  m4 Transform::GetParentModelMatrix() {
    return _parent_model_matrix;
  }

  bool Transform::WasDirty() {
    bool was_dirty = _was_dirty;
    _was_dirty = false;
    return was_dirty;
  }

  bool Transform::ShowComponent() {
    bool modified = false;

    if (ShowData("Position", _position))
      modified = true;
    if (ShowData("Scale", _scale, v3(1.0f)))
      modified = true;
    if (ShowData("Rotation", _rotation))
      modified = true;

    if (modified)
      _is_dirty = true;

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

  void Transform::Deserialize(const json &json) { }

} // namespace axl
