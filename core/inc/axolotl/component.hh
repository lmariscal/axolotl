#pragma once

#include <axolotl/types.hh>
#include <entt/entt.hpp>
#include <imgui.h>
#include <limits>
#include <uuid.h>
#include <vector>

namespace axl {

  // Everything is linked to a scene, a scene is the root how everything is
  // serialized and organized in a structured tree.
  // We need to be able to serialize and deserialize a scene, which includes all of
  // its components and its location in the tree, including parent and children.
  // We need a way to identify each entity, so we are able to properly deserialize into
  // the correct position in the tree.
  // I think the parent and children are going to be managed by the transform component, since
  // that is the only component that is affected by the parent and child relationships.

  // I am aware that I can use to_json and from_json to serialize and deserialize. The issue is
  // that these functions are not fully under my control and prefer to have a dedicated Serialize
  // and Deserialize function.
  class Ento;

  bool ShowData(const std::string &label, v2 &v, const v2 &reset_values = { 0.0f, 0.0f });
  bool ShowData(const std::string &label,
                v3 &v,
                const v3 &reset_values = { 0.0f, 0.0f, 0.0f },
                f32 min = 0.0f,
                f32 max = 0.0f);
  bool ShowData(const std::string &label, v4 &v, const v4 &reset_values = { 0.0f, 0.0f, 0.0f, 0.0f });
  bool ShowDataColor(const std::string &label, v4 &v);
  bool ShowData(const std::string &label, quat &v, const v3 &reset_values = { 0.0f, 0.0f, 0.0f });
  bool ShowData(const std::string &label, f32 &v, const f32 &reset_value = 0.0f, f32 min = 0.0f, f32 max = 0.0f);
  bool ShowData(const std::string &label, bool &v);
  bool ShowData(const std::string &label, i32 &v, const i32 &reset_value = 0, i32 min = 0, i32 max = 0);
  bool ShowData(const std::string &label, u32 &v, const u32 &reset_value = 0, u32 min = 0, u32 max = 0);
  bool ShowDataTexture(const std::string &label, i32 texture_id);
} // namespace axl

namespace nlohmann {

  template<typename T>
  struct adl_serializer<axl::vec<2, T>> {
    static void to_json(json &j, const axl::vec<2, T> &v) {
      j = json { { "x", v.x }, { "y", v.y } };
    }

    static void from_json(const json &j, axl::vec<2, T> &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
    }
  };

  template<typename T>
  struct adl_serializer<axl::vec<3, T>> {
    static void to_json(json &j, const axl::vec<3, T> &v) {
      j = json { { "x", v.x }, { "y", v.y }, { "z", v.z } };
    }

    static void from_json(const json &j, axl::vec<3, T> &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
      v.z = j.at("z").get<axl::f32>();
    }
  };

  template<typename T>
  struct adl_serializer<axl::vec<4, T>> {
    static void to_json(json &j, const axl::vec<4, T> &v) {
      j = json { { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
    }

    static void from_json(const json &j, axl::vec<4, T> &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
      v.z = j.at("z").get<axl::f32>();
      v.w = j.at("w").get<axl::f32>();
    }
  };

  template<>
  struct adl_serializer<axl::quat> {
    static void to_json(json &j, const axl::quat &v) {
      j = json { { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
    }

    static void from_json(const json &j, axl::quat &v) {
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
      v.z = j.at("z").get<axl::f32>();
      v.w = j.at("w").get<axl::f32>();
    }
  };

} // namespace nlohmann
