#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <spdlog/spdlog.h>
#include <cstdint>

namespace axl {

  using namespace glm;

  using i8  = std::int8_t;
  using i16 = std::int16_t;
  using i32 = std::int32_t;
  using i64 = std::int64_t;

  using u8  = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using u64 = std::uint64_t;

  using f32 = float ;
  using f64 = double;

  using uchar = unsigned char;

  using v2 = vec2;
  using v3 = vec3;
  using v4 = vec4;

  using v2i = vec<2, i32>;
  using v2d = vec<2, f64>;
  using v3i = vec<3, i32>;
  using v3d = vec<3, f64>;
  using v4i = vec<4, i32>;
  using v4d = vec<4, f64>;

  using m2 = mat2;
  using m3 = mat3;
  using m4 = mat4;

#ifndef M_PI
  constexpr f32 M_PI = 3.14159265358979323846;
#endif

  namespace log {
    using namespace spdlog;
  }

}

#define IM_VEC2_CLASS_EXTRA                                                 \
        ImVec2(const axl::v2& f) { x = f.x; y = f.y; }                       \
        operator axl::v2() const { return axl::v2(x,y); }
#define IM_VEC4_CLASS_EXTRA                                                 \
        ImVec4(const axl::v4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }     \
        operator axl::v4() const { return axl::v4(x,y,z,w); }
