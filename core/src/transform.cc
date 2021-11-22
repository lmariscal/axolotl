#include <axolotl/transform.h>
#include <imgui.h>

namespace axl {

  Transform::Transform():
    _position({ 0.0f, 0.0f, 0.0f }),
    _scale({ 1.0f, 1.0f, 1.0f }),
    _rotation()
  {

  }

  Transform::~Transform() {

  }

  Transform::Transform(const v3& position, const v3& scale, const quat& rotation):
    _position(position),
    _scale(scale),
    _rotation(rotation)
  {
  }

  Transform::Transform(const Transform& other):
    _position(other._position),
    _scale(other._scale),
    _rotation(other._rotation)
  {
  }

  const v3& Transform::GetPosition() const {
    return _position;
  }

  const v3& Transform::GetScale() const {
    return _scale;
  }

  const quat& Transform::GetRotation() const {
    return _rotation;
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

  nlohmann::json Transform::Serialize() const {
    nlohmann::json j;
    j["version"] = {
      { "major", _version_major },
      { "minor", _version_minor }
    };
    j["type"] = "transform";
    j["position"] = _position;
    j["scale"] = _scale;
    j["rotation"] = _rotation;
    return j;
  }

  void Transform::Deserialize(const nlohmann::json &json) {
    if (json.find("version") != json.end()) {
      if (json["version"]["major"] != _version_major) {
        log::error("Transform::Deserialize: incompatible version");
        return;
      }
    }

    if (json.find("type") != json.end()) {
      if (json["type"] != "transform") {
        log::error("Transform::Deserialize: invalid type");
        return;
      }
    }

    if (json.find("position") != json.end()) {
      _position = json["position"];
    }
    if (json.find("scale") != json.end()) {
      _scale = json["scale"];
    }
    if (json.find("rotation") != json.end()) {
      _rotation = json["rotation"];
    }
  }

  void Transform::ShowDataToUI() {
    if (ImGui::CollapsingHeader("Transform")) {
      serializable::ShowDataToUI("Position", _position);
      serializable::ShowDataToUI("Scale", _scale, v3(1.0f));
    }
  }

} // namespace axl
