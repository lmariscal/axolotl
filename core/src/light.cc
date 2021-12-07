#include <axolotl/light.hh>

#include <axolotl/component.hh>

#include <imgui.h>

namespace axl {

  Light::Light(LightType type, v3 color, f32 intensity):
    color(v4(color, 1.0f)),
    intensity(intensity),
    type(type)
  {
  }

  v3 Light::GetColorRGB() {
    return color * 255.0f;
  }

  void Light::SetColorRGB(const v3& color) {
    this->color = vec4(color / 255.0f, 1.0f);
  }

  void Light::SetColor(const v3 &color) {
    this->color = vec4(color, 1.0f);
  }

  bool Light::ShowComponent() {
    bool modified = false;

    if (ShowDataColor("Color", color))
      modified = true;
    if (ShowData("Intensity", intensity))
      modified = true;

    return modified;
  }

} // namespace axl