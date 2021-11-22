#pragma once

#include <axolotl/scene.h>
#include <nlohmann/json.hpp>

namespace axl {

  // Everything is linked to a scene, a scene is the root how everything is
  // serialized and organized in a structured tree.
  // We need to be able to serialize and deserialize a scene, which includes all of
  // its components and its location in the tree, including parent and children.
  // We need a way to identify each entity, so we are able to properly deserialize into
  // the correct position in the tree.
  // I think the parent and children are going to be managed by the transform component, since
  // that is the only component that is affected by the parent and child relationships.

  // We have to also think about prefabs, which are a way to create entities with prefilled data and
  // behaviour. Maybe we can manage parent and children relationship for transformation in the
  // transform component, and then we can manage prefabs as a special kind of entity, which is a
  // scene that is attachable to other scenes.

  // I am aware that I can use to_json and from_json to serialize and deserialize. The issue is
  // that these functions are not fully under my control and prefer to have a dedicated Serialize
  // and Deserialize function.

  class Serializable {
   public:
    virtual nlohmann::json Serialize() const = 0;
    virtual void Deserialize(const nlohmann::json &json) = 0;

   protected:
    u32 _version_major = 0;
    u32 _version_minor = 1;
  };

} // namespace axl

namespace nlohmann {

  template <typename T>
  struct adl_serializer<axl::vec<2, T>> {
    static void to_json(json &j, const axl::vec<2, T> &v) {
      j = json{{"x", v.x}, {"y", v.y}};
    }

    static void from_json(const json &j, axl::vec<2, T> &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
    }
  };

  template <typename T>
  struct adl_serializer<axl::vec<3, T>> {
    static void to_json(json &j, const axl::vec<3, T> &v) {
      j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}};
    }

    static void from_json(const json &j, axl::vec<3, T> &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
      v.z = j.at("z").get<axl::f32>();
    }
  };

  template <typename T>
  struct adl_serializer<axl::vec<4, T>> {
    static void to_json(json &j, const axl::vec<4, T> &v) {
      j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
    }

    static void from_json(const json &j, axl::vec<4, T> &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
      v.z = j.at("z").get<axl::f32>();
      v.w = j.at("w").get<axl::f32>();
    }
  };

  template <>
  struct adl_serializer<axl::quat> {
    static void to_json(json &j, const axl::quat &v) {
      j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
    }

    static void from_json(const json &j, axl::quat &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
      v.z = j.at("z").get<axl::f32>();
      v.w = j.at("w").get<axl::f32>();
    }
  };

} // namespace nlohmann
