#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <ergo/assert.hh>
#include <ergo/types.hh>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <uuid.h>

namespace axl {

  using namespace glm;
  using namespace ergo;

  using v2 = vec2;
  using v3 = vec3;
  using v4 = vec4;

  using v2i = vec<2, i32>;
  using v2u = vec<2, u32>;
  using v2d = vec<2, f64>;
  using v3i = vec<3, i32>;
  using v3d = vec<3, f64>;
  using v4i = vec<4, i32>;
  using v4d = vec<4, f64>;

  using m2 = mat2;
  using m3 = mat3;
  using m4 = mat4;

  using json = nlohmann::json;
  using uuid = uuids::uuid;

  namespace log {
    using namespace spdlog;
  }

  // Stores color information as a normalized v4
  // Not using `using` because we want the RGB and HSV functions (TODO: HSV)
  class Color {
   public:
    v4 rgba;

    Color(): rgba(1.0f) { }
    Color(f32 r, f32 g, f32 b): rgba(r, g, b, 1.0f) { }
    Color(f32 r, f32 g, f32 b, f32 a): rgba(r, g, b, a) { }
    Color(const v3 &rgb): rgba(v4(rgb, 1.0f)) { }
    Color(const v4 &rgba): rgba(rgba) { }

    void SetRGB(f32 r, f32 g, f32 b) {
      rgba = v4(r, g, b, rgba.a);
    }

    void SetRGBA(f32 r, f32 g, f32 b, f32 a) {
      rgba = v4(r, g, b, a);
    }

    void SetRGB(const v3 &rgb) {
      rgba = v4(rgb, rgba.a);
    }

    void SetRGBA(const v4 &rgba) {
      this->rgba = rgba;
    }

    operator v3() const {
      return rgba;
    }

    operator v4() const {
      return rgba;
    }

    Color &operator=(const v3 &rgb) {
      rgba = v4(rgb, 1.0f);
      return *this;
    }

    Color &operator=(const v4 &rgba) {
      this->rgba = rgba;
      return *this;
    }

    Color &operator+=(const Color &other) {
      rgba += other.rgba;
      return *this;
    }

    Color &operator-=(const Color &other) {
      rgba -= other.rgba;
      return *this;
    }

    Color &operator*=(f32 s) {
      rgba *= s;
      return *this;
    }

    Color &operator/=(f32 s) {
      rgba /= s;
      return *this;
    }

    Color operator+(const Color &other) const {
      return Color(rgba + other.rgba);
    }

    Color operator-(const Color &other) const {
      return Color(rgba - other.rgba);
    }

    Color operator*(f32 s) const {
      return Color(rgba * s);
    }

    Color operator/(f32 s) const {
      return Color(rgba / s);
    }

    Color operator-() const {
      return Color(-rgba);
    }

    bool operator==(const Color &other) const {
      return rgba == other.rgba;
    }

    bool operator!=(const Color &other) const {
      return rgba != other.rgba;
    }
  };

} // namespace axl

#define IM_VEC2_CLASS_EXTRA  \
  ImVec2(const axl::v2 &f) { \
    x = f.x;                 \
    y = f.y;                 \
  }                          \
  operator axl::v2() const { \
    return axl::v2(x, y);    \
  }
#define IM_VEC4_CLASS_EXTRA     \
  ImVec4(const axl::v4 &f) {    \
    x = f.x;                    \
    y = f.y;                    \
    z = f.z;                    \
    w = f.w;                    \
  }                             \
  operator axl::v4() const {    \
    return axl::v4(x, y, z, w); \
  }

#define AXL_ASSERT_MESSAGE(validate, ...) ERGO_ASSERT_MESSAGE(validate, __VA_ARGS__)
#define AXL_ASSERT(validate)              ERGO_ASSERT(validate)
