#include <axolotl/serialization.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace axl {

  namespace serializable {

    bool ShowDataToUI(const std::string &label, v3 &v, const v3 &reset_value) {
      i32 column_width = 100;
      bool modified = false;

      ImGuiIO& io = ImGui::GetIO();
      auto bold_font = io.Fonts->Fonts[1];

      ImGui::PushID(label.c_str());

      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, column_width);
      ImGui::Text("%s", label.c_str());
      ImGui::NextColumn();

      ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2{ 0, 0 });

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
      if (ImGui::DragFloat("##X", &v.x, 0.1f, 0.0f, 0.0f, "%.2f"))
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
      if (ImGui::DragFloat("##Y", &v.y, 0.1f, 0.0f, 0.0f, "%.2f"))
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
      if (ImGui::DragFloat("##Z", &v.z, 0.1f, 0.0f, 0.0f, "%.2f"))
        modified = true;
      ImGui::PopItemWidth();

      ImGui::PopStyleVar();

      ImGui::Columns(1);

      ImGui::PopID();
      return modified;
    }

    bool ShowDataToUI(const std::string &label, v2 &v, const v2 &reset_value) {
      i32 column_width = 100;
      bool modified = false;

      ImGuiIO& io = ImGui::GetIO();
      auto bold_font = io.Fonts->Fonts[1];

      ImGui::PushID(label.c_str());

      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, column_width);
      ImGui::Text("%s", label.c_str());
      ImGui::NextColumn();

      ImGui::PushMultiItemsWidths(2, (2 * ImGui::CalcItemWidth() / 3) - 1);
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2{ 0, 0 });

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
      if (ImGui::DragFloat("##X", &v.x, 0.1f, 0.0f, 0.0f, "%.2f"))
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
      if (ImGui::DragFloat("##Y", &v.y, 0.1f, 0.0f, 0.0f, "%.2f"))
        modified = true;
      ImGui::PopItemWidth();

      ImGui::PopStyleVar();

      ImGui::Columns(1);

      ImGui::PopID();
      return modified;
    }

    bool ShowDataToUI(const std::string &label, v4 &v, const v4 &reset_value) {
      i32 column_width = 100;
      bool modified = false;

      ImGuiIO& io = ImGui::GetIO();
      auto bold_font = io.Fonts->Fonts[1];

      ImGui::PushID(label.c_str());

      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, column_width);
      ImGui::Text("%s", label.c_str());
      ImGui::NextColumn();

      ImGui::PushMultiItemsWidths(4, (4 * ImGui::CalcItemWidth() / 3) - 1);
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2{ 0, 0 });

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
      if (ImGui::DragFloat("##X", &v.x, 0.1f, 0.0f, 0.0f, "%.2f"))
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
      if (ImGui::DragFloat("##Y", &v.y, 0.1f, 0.0f, 0.0f, "%.2f"))
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
      if (ImGui::DragFloat("##Z", &v.z, 0.1f, 0.0f, 0.0f, "%.2f"))
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
      if (ImGui::DragFloat("##W", &v.w, 0.1f, 0.0f, 0.0f, "%.2f"))
        modified = true;
      ImGui::PopItemWidth();

      ImGui::PopStyleVar();

      ImGui::Columns(1);

      ImGui::PopID();
      return modified;
    }

    bool ShowDataToUI(const std::string &label, quat &v, const v3 &reset_values) {
      v3 euler = degrees(eulerAngles(v));
      if (!ShowDataToUI(label, euler, reset_values))
        return false;
      v = quat(radians(euler));
      return true;
    }

  } // namespace axl::serializable

} // namespace axl
