#pragma once

#include <axolotl/component.hh>
#include <axolotl/types.hh>

namespace axl {

  class Transform {
   public:
    Transform();
    Transform(const v3 &position, const v3 &scale = v3(1.0f), const quat &rotation = quat());
    Transform(const Transform &other) = default;
    Transform(Transform &&other) = default;
    Transform &operator=(const Transform &other) = default;
    Transform &operator=(Transform &&other) = default;

    void Init();
    json Serialize() const;
    void Deserialize(const json &json);
    bool ShowComponent();

    const v3 &GetPosition() const;
    const v3 &GetScale() const;
    const quat &GetRotationQuat() const;
    const v3 GetRotation() const;

    void SetPosition(const v3 &position);
    void SetScale(const v3 &scale);
    void SetRotation(const quat &rotation);
    void SetRotation(const v3 &rotation);

    m4 GetModelMatrix(Ento ento) const;

    REGISTER_COMPONENT(Transform, _position, _scale, _rotation);

   protected:
    v3 _position;
    v3 _scale;
    quat _rotation;
  };

} // namespace axl
