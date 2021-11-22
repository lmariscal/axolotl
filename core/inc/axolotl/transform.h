#pragma once

#include <axolotl/types.h>
#include <axolotl/component.h>

namespace axl {

  struct Transform : public Component {
   public:
    Transform();
    Transform(const v3& position, const v3& scale = v3(1.0f), const quat& rotation = quat());
    Transform(const Transform& other);
    ~Transform();

    virtual json Serialize() const;
    virtual void Deserialize(const json &json);
    virtual bool ShowData();
    virtual void Init();

    const v3 & GetPosition() const;
    const v3 & GetScale() const;
    const quat & GetRotationQuat() const;
    const v3 GetRotation() const;

    void SetPosition(const v3 &position);
    void SetScale(const v3 &scale);
    void SetRotation(const quat &rotation);
    void SetRotation(const v3 &rotation);

    m4 GetModelMatrix() const;

   protected:
    v3 _position;
    v3 _scale;
    quat _rotation;
  };

} // namespace axl
