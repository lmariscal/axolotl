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

  // enum class LightType { Ambient, Directional, Point, Spot, Last };
  std::string LightTypeToString(LightType type) {
    switch (type) {
      case LightType::Ambient:
        return "Ambient";
      case LightType::Directional:
        return "Directional";
      case LightType::Point:
        return "Point";
      case LightType::Spot:
        return "Spot";
      default:
        return "Unknown";
    }
  }

  bool Light::ShowComponent() {
    bool modified = false;

    ImGui::Text("Type");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##type", LightTypeToString(type).c_str())) {
      for (i32 i = 0; i < (i32)LightType::Last; i++) {
        bool is_selected = (i == (i32)type);
        if (ImGui::Selectable(LightTypeToString((LightType)i).c_str(), is_selected)) {
          type = (LightType)i;
          modified = true;
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    if (ShowData("Color", color))
      modified = true;
    if (ShowData("Intensity", intensity))
      modified = true;

    return modified;
  }

} // namespace axl
