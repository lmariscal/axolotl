#pragma once

#include <axolotl/component.hh>
#include <axolotl/types.hh>

namespace axl {

  constexpr u32 LIGHT_COUNT = 32;

  enum class LightType { Ambient, Directional, Point, Spot, Last };

  class Light {
   public:
    Light();
    Light(LightType type, v3 color = v3(1.0f), f32 intensity = 1.0f);

    void Init();
    v3 GetColorRGB();
    void SetColorRGB(const v3 &color);
    void SetColor(const v3 &color);

    bool ShowComponent();

    REGISTER_COMPONENT(Light, color, intensity, type)

    Color color;
    f32 intensity;
    LightType type;
  };

  class LightData {
   public:
    v4 position;
    v4 color;
    f32 intensity;
    v3 offset3 = v3(0.0f);
  };

} // namespace axl
