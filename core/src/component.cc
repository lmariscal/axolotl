#include <axolotl/component.hh>
#include <axolotl/scene.hh>
#include <axolotl/texture.hh>
#include <imgui.h>
#include <imgui_internal.h>

namespace axl {

  constexpr i32 column_width = 120;

  bool ShowData(const std::string &label, v3 &v, const v3 &reset_value, f32 min, f32 max) {
    bool modified = false;

    ImGuiIO &io = ImGui::GetIO();
    auto bold_font = io.Fonts->Fonts[1];

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width * 3 && label_size < column_width) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2 { 0, 0 });

    f32 lineHeight = 20.0f + 1.0f * 2.0f;
    v2 buttonSize = { lineHeight + 3.0f, lineHeight };

    v4 x_color = v4(211, 47, 47, 0);
    x_color /= 255.0f;
    x_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, x_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, x_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, x_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("X", buttonSize)) {
      v.x = reset_value.x;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &v.x, 0.1f, min, max, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    v4 y_color = v4(56, 142, 60, 0);
    y_color /= 255.0f;
    y_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, y_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, y_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, y_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Y", buttonSize)) {
      v.y = reset_value.y;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &v.y, 0.1f, min, max, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    v4 z_color = v4(25, 118, 210, 0);
    z_color /= 255.0f;
    z_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, z_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, z_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, z_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Z", buttonSize)) {
      v.z = reset_value.z;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &v.z, 0.1f, min, max, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, v2 &v, const v2 &reset_value) {
    bool modified = false;

    ImGuiIO &io = ImGui::GetIO();
    auto bold_font = io.Fonts->Fonts[1];

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width * 2 && label_size < column_width) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(2, (2 * ImGui::CalcItemWidth() / 3) - 1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2 { 0, 0 });

    f32 lineHeight = 20.0f + 1.0f * 2.0f;
    v2 buttonSize = { lineHeight + 3.0f, lineHeight };

    v4 x_color = v4(211, 47, 47, 0);
    x_color /= 255.0f;
    x_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, x_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, x_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, x_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("X", buttonSize)) {
      v.x = reset_value.x;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &v.x, 0.1f, 0.0f, 0.0f, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    v4 y_color = v4(56, 142, 60, 0);
    y_color /= 255.0f;
    y_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, y_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, y_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, y_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Y", buttonSize)) {
      v.y = reset_value.y;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &v.y, 0.1f, 0.0f, 0.0f, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, v4 &v, const v4 &reset_value) {
    bool modified = false;

    ImGuiIO &io = ImGui::GetIO();
    auto bold_font = io.Fonts->Fonts[1];

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width * 4 && label_size < column_width) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(4, (4 * ImGui::CalcItemWidth() / 3) - 1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2 { 0, 0 });

    f32 lineHeight = 20.0f + 1.0f * 2.0f;
    v2 buttonSize = { lineHeight + 3.0f, lineHeight };

    v4 x_color = v4(211, 47, 47, 0);
    x_color /= 255.0f;
    x_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, x_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, x_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, x_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("X", buttonSize)) {
      v.x = reset_value.x;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &v.x, 0.1f, 0.0f, 0.0f, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    v4 y_color = v4(56, 142, 60, 0);
    y_color /= 255.0f;
    y_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, y_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, y_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, y_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Y", buttonSize)) {
      v.y = reset_value.y;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &v.y, 0.1f, 0.0f, 0.0f, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();

    ImGui::SetCursorPosX(100.0f + 7.5f);
    v4 z_color = v4(25, 118, 210, 0);
    z_color /= 255.0f;
    z_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, z_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, z_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, z_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("Z", buttonSize)) {
      v.z = reset_value.z;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##Z", &v.z, 0.1f, 0.0f, 0.0f, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    v4 w_color = v4(123, 31, 162, 0);
    w_color /= 255.0f;
    w_color.w = 1.0f;
    ImGui::PushStyleColor(ImGuiCol_Button, w_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, w_color * 1.2f);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, w_color);
    ImGui::PushFont(bold_font);
    if (ImGui::Button("W", buttonSize)) {
      v.w = reset_value.w;
      modified = true;
    }
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    if (ImGui::DragFloat("##W", &v.w, 0.1f, 0.0f, 0.0f, "%.3f"))
      modified = true;
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, quat &v, const v3 &reset_values) {
    v3 original = degrees(eulerAngles(v));

    v3 euler = original;
    if (euler.x <= 0.001f && euler.x >= -0.001f)
      euler.x = 0.0f;
    if (euler.y <= 0.001f && euler.y >= -0.001f)
      euler.y = 0.0f;
    if (euler.z <= 0.001f && euler.z >= -0.001f)
      euler.z = 0.0f;
    if (!ShowData(label, euler, reset_values))
      return false;

    quat offset = quat(radians(euler - original));
    if (length2(v) == 0.0f) {
      v = offset;
    } else {
      v = v * offset;
    }

    return true;
  }

  // bool ShowData(const std::string &label, f32 &v, const f32 &reset_value = 0.0f);
  // bool ShowData(const std::string &label, bool &v);

  bool ShowData(const std::string &label, f64 &v, const f64 &reset_value, f64 min, f64 max) {
    bool modified = false;

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width && label_size < column_width * 3) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    f32 vt = v;
    if (ImGui::DragFloat("##v", &vt, 0.1f, min, max, "%.3f")) {
      modified = true;
      v = vt;
    }
    ImGui::Columns(1);
    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, f32 &v, const f32 &reset_value, f32 min, f32 max) {
    bool modified = false;

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width && label_size < column_width * 3) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    if (ImGui::DragFloat("##v", &v, 0.1f, min, max, "%.3f"))
      modified = true;

    ImGui::Columns(1);
    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, bool &v) {
    bool modified = false;

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width && label_size < column_width) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    if (ImGui::Checkbox("##v", &v))
      modified = true;

    ImGui::Columns(1);
    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, i32 &v, const i32 &reset_value, i32 min, i32 max) {
    bool modified = false;

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width && label_size < column_width * 3) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    if (ImGui::DragInt("##v", &v, 0.1f, min, max))
      modified = true;

    ImGui::Columns(1);
    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, GLTexture texture) {
    bool modified = false;

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    ImGui::Text("%s", label.c_str());

    f32 size = ImGui::CalcItemWidth();
    ImGui::Image((void *)(intptr_t)texture.id, v2(size));

    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, u32 &v, const u32 &reset_value, u32 min, u32 max) {
    bool modified = false;

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width && label_size < column_width * 3) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    i32 tmp;
    if (ImGui::DragInt("##v", &tmp, 0.1f, min, max)) {
      modified = true;
      v = tmp;
    }

    ImGui::Columns(1);
    ImGui::PopID();
    return modified;
  }

  bool ShowData(const std::string &label, Color &v) {
    bool modified = false;

    ImGuiIO &io = ImGui::GetIO();
    auto bold_font = io.Fonts->Fonts[1];

    ImGui::PushID(label.c_str());

    f32 label_size = ImGui::CalcTextSize(label.c_str()).x + 15;
    if (ImGui::GetWindowWidth() > column_width * 4 && label_size < column_width) {
      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, label_size > column_width ? label_size : column_width);
    } else {
      ImGui::Columns(1);
    }

    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushItemWidth(ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2 { 0, 0 });

    ImGui::PushFont(bold_font);
    // imgui button position on the left
    if (ImGui::ColorEdit4("##color", value_ptr(v.rgba)))
      modified = true;
    ImGui::PopFont();

    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();

    return modified;
  }

} // namespace axl
