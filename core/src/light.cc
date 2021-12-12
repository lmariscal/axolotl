#include <axolotl/component.hh>
#include <axolotl/light.hh>
#include <imgui.h>

namespace axl {

  Light::Light(): color(v4(1.0f)), intensity(1.0f), type(LightType::Directional) { }

  Light::Light(LightType type, v3 color, f32 intensity): color(v4(color, 1.0f)), intensity(intensity), type(type) { }

  void Light::Init() { }

  v3 Light::GetColorRGB() {
    return color * 255.0f;
  }

  void Light::SetColorRGB(const v3 &color) {
    this->color = vec4(color / 255.0f, 1.0f);
  }

  void Light::SetColor(const v3 &color) {
    this->color = vec4(color, 1.0f);
  }

  bool Light::ShowComponent() {
    bool modified = false;

    if (ShowData("Color", color)) modified = true;
    if (ShowData("Intensity", intensity)) modified = true;

    return modified;
  }

} // namespace axl
