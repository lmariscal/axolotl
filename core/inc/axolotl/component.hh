#pragma once

#include <axolotl/types.hh>
#include <entt/entt.hpp>
#include <imgui.h>
#include <limits>
#include <nlohmann/json.hpp>
#include <uuid.h>
#include <vector>

#define COMPONENT_STRINGIZE(arg)  COMPONENT_STRINGIZE1(arg)
#define COMPONENT_STRINGIZE1(arg) COMPONENT_STRINGIZE2(arg)
#define COMPONENT_STRINGIZE2(arg) #arg

#define COMPONENT_CONCATENATE(arg1, arg2)  COMPONENT_CONCATENATE1(arg1, arg2)
#define COMPONENT_CONCATENATE1(arg1, arg2) COMPONENT_CONCATENATE2(arg1, arg2)
#define COMPONENT_CONCATENATE2(arg1, arg2) arg1##arg2

#define COMPONENT_FOR_EACH_1(what, x, y, ...) what(x, y)
#define COMPONENT_FOR_EACH_2(what, x, y, ...) what(x, y) COMPONENT_FOR_EACH_1(what, x, __VA_ARGS__)
#define COMPONENT_FOR_EACH_3(what, x, y, ...) what(x, y) COMPONENT_FOR_EACH_2(what, x, __VA_ARGS__)
#define COMPONENT_FOR_EACH_4(what, x, y, ...) what(x, y) COMPONENT_FOR_EACH_3(what, x, __VA_ARGS__)
#define COMPONENT_FOR_EACH_5(what, x, y, ...) what(x, y) COMPONENT_FOR_EACH_4(what, x, __VA_ARGS__)
#define COMPONENT_FOR_EACH_6(what, x, y, ...) what(x, y) COMPONENT_FOR_EACH_5(what, x, __VA_ARGS__)
#define COMPONENT_FOR_EACH_7(what, x, y, ...) what(x, y) COMPONENT_FOR_EACH_6(what, x, __VA_ARGS__)
#define COMPONENT_FOR_EACH_8(what, x, y, ...) what(x, y) COMPONENT_FOR_EACH_7(what, x, __VA_ARGS__)

#define COMPONENT_FOR_EACH_NARG(...)                                     COMPONENT_FOR_EACH_NARG_(__VA_ARGS__, COMPONENT_FOR_EACH_RSEQ_N())
#define COMPONENT_FOR_EACH_NARG_(...)                                    COMPONENT_FOR_EACH_ARG_N(__VA_ARGS__)
#define COMPONENT_FOR_EACH_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define COMPONENT_FOR_EACH_RSEQ_N()                                      8, 7, 6, 5, 4, 3, 2, 1, 0

#define COMPONENT_FOR_EACH_(N, what, x, y, ...) COMPONENT_CONCATENATE(COMPONENT_FOR_EACH_, N)(what, x, y, __VA_ARGS__)
#define COMPONENT_FOR_EACH(what, x, ...)        COMPONENT_FOR_EACH_(COMPONENT_FOR_EACH_NARG(__VA_ARGS__), what, x, __VA_ARGS__)

#define REGISTER_COMPONENT_MEMBER(Type, Member) \
  .data<&Type::Member>(entt::hashed_string::value(#Member)).prop("name"_hs, std::string(#Member))

#define REGISTER_COMPONENT_FACTORY(Type, ...)                                                                      \
  entt::meta<Type>()                                                                                               \
    .type()                                                                                                        \
    .prop("name"_hs, std::string(#Type))                                                                           \
    .func<static_cast<Type &(entt::registry::*)(const entt::entity)>(&entt::registry::get<Type>), entt::as_ref_t>( \
      "get"_hs)                                                                                                    \
    .func<&entt::registry::emplace_or_replace<Type>, entt::as_ref_t>("emplace"_hs)                                 \
    .func<&Type::GetHash>("GetHash"_hs)                                                                            \
    .func<&Type::Init>("Init"_hs) COMPONENT_FOR_EACH(REGISTER_COMPONENT_MEMBER, Type, __VA_ARGS__)

#define REGISTER_COMPONENT_DATA_TYPE(Type) \
  entt::meta<Type>().type().conv<json>().func<&DefaultFromJson<Type>>("FromJSON"_hs)
#define REGISTER_COMPONENT_DATA_TYPE_CTOR(Type, ...) \
  entt::meta<Type>().type().ctor<__VA_ARGS__>().conv<json>().func<&DefaultFromJson<Type>>("FromJSON"_hs)

#define REGISTER_COMPONENT(Type, ...)                \
  static i32 RegisterComponent() {                   \
    using namespace entt::literals;                  \
    REGISTER_COMPONENT_FACTORY(Type, __VA_ARGS__);   \
    auto meta = entt::resolve<Type>();               \
    std::string s = std::string(meta.info().name()); \
    for (auto data : meta.data())                    \
      s += data.type().info().name();                \
    return entt::hashed_string::value(s.c_str());    \
  }                                                  \
  inline static MetaHolder<Type> _meta;              \
  static i32 GetHash() {                             \
    return _meta.component_hash;                     \
  }

namespace axl {

  class GLTexture;

  template<typename T>
  static void DefaultFromJson(const json &j, T &t) {
    t = j.get<T>();
  }

  template<typename T>
  class MetaHolder {
   public:
    inline static i32 component_hash = T::RegisterComponent();
  };

  class Ento;

  bool ShowData(const std::string &label, v2 &v, const v2 &reset_values = { 0.0f, 0.0f });
  bool ShowData(const std::string &label,
                v3 &v,
                const v3 &reset_values = { 0.0f, 0.0f, 0.0f },
                f32 min = 0.0f,
                f32 max = 0.0f);
  bool ShowData(const std::string &label, v4 &v, const v4 &reset_values = { 0.0f, 0.0f, 0.0f, 0.0f });
  bool ShowData(const std::string &label, Color &v);
  bool ShowData(const std::string &label, quat &v, const v3 &reset_values = { 0.0f, 0.0f, 0.0f });
  bool ShowData(const std::string &label, f32 &v, const f32 &reset_value = 0.0f, f32 min = 0.0f, f32 max = 0.0f);
  bool ShowData(const std::string &label, bool &v);
  bool ShowData(const std::string &label, i32 &v, const i32 &reset_value = 0, i32 min = 0, i32 max = 0);
  bool ShowData(const std::string &label, u32 &v, const u32 &reset_value = 0, u32 min = 0, u32 max = 0);
  bool ShowData(const std::string &label, GLTexture texture_id);
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

  template<>
  struct adl_serializer<axl::Color> {
    static void to_json(json &j, const axl::Color &c) {
      const axl::v4 &v = c.rgba;
      j = json { { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
    }

    static void from_json(const json &j, axl::Color &c) {
      axl::v4 &v = c.rgba;
      v.x = j.at("x").get<axl::f32>();
      v.y = j.at("y").get<axl::f32>();
      v.z = j.at("z").get<axl::f32>();
      v.w = j.at("w").get<axl::f32>();
    }
  };

  template<>
  struct adl_serializer<axl::uuid> {
    static void to_json(json &j, const axl::uuid &u) {
      j = uuids::to_string(u);
    }

    static void from_json(const json &j, axl::uuid &u) {
      std::optional<axl::uuid> ou = axl::uuid::from_string(j.get<std::string>());
      if (ou.has_value())
        u = ou.value();
      else
        axl::log::error("Failed to parse uuid from json {}", j.dump());
    }
  };

} // namespace nlohmann
