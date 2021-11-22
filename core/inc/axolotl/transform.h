#pragma once

#include <axolotl/types.h>
#include <axolotl/serialization.h>

namespace axl {

  struct Transform : public Serializable {
   public:
    Transform();
    Transform(const v3& position, const v3& scale, const quat& rotation);
    Transform(const Transform& other);
    ~Transform();

    virtual nlohmann::json Serialize() const;
    virtual void Deserialize(const nlohmann::json &json);

    const v3& GetPosition() const;
    const v3& GetScale() const;
    const quat& GetRotation() const;

    void SetPosition(const v3& position);
    void SetScale(const v3& scale);
    void SetRotation(const quat& rotation);

   protected:
    v3 _position;
    v3 _scale;
    quat _rotation;
  };

} // namespace axl
