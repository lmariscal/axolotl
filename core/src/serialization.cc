#include <axolotl/serialization.h>

#include <imgui.h>
#include <imgui_internal.h>

namespace axl {

  namespace serializable {

    void ShowDataToUI(const std::string &label, v2 &v, const v2 &reset_value) {
      ImGui::Text("%s", label.c_str());
      ImGui::SameLine();
      ImGui::InputFloat2(std::string("##v2" + label).c_str(), &v.x);
    }

    void ShowDataToUI(const std::string &label, v3 &v, const v3 &reset_value) {
      i32 column_width = 100;

      ImGuiIO& io = ImGui::GetIO();

      ImGui::PushID(label.c_str());

      ImGui::Columns(2);
      ImGui::SetColumnWidth(0, column_width);
      ImGui::Text("%s", label.c_str());
      ImGui::NextColumn();

      ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2{ 0, 0 });

      f32 lineHeight = 20.0f + 1.0f * 2.0f;
      v2 buttonSize = { lineHeight + 3.0f, lineHeight };

      ImGui::PushStyleColor(ImGuiCol_Button, v4{ 0.8f, 0.1f, 0.15f, 1.0f });
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, v4{ 0.9f, 0.2f, 0.2f, 1.0f });
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, v4{ 0.8f, 0.1f, 0.15f, 1.0f });
      if (ImGui::Button("X", buttonSize))
        v.x = reset_value.x;
      ImGui::PopStyleColor(3);

      ImGui::SameLine();
      ImGui::DragFloat("##X", &v.x, 0.1f, 0.0f, 0.0f, "%.2f");
      ImGui::PopItemWidth();
      ImGui::SameLine();

      ImGui::PushStyleColor(ImGuiCol_Button, v4{ 0.2f, 0.7f, 0.2f, 1.0f });
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, v4{ 0.3f, 0.8f, 0.3f, 1.0f });
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, v4{ 0.2f, 0.7f, 0.2f, 1.0f });
      if (ImGui::Button("Y", buttonSize))
        v.y = reset_value.y;
      ImGui::PopStyleColor(3);

      ImGui::SameLine();
      ImGui::DragFloat("##Y", &v.y, 0.1f, 0.0f, 0.0f, "%.2f");
      ImGui::PopItemWidth();
      ImGui::SameLine();

      ImGui::PushStyleColor(ImGuiCol_Button, v4{ 0.1f, 0.25f, 0.8f, 1.0f });
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, v4{ 0.2f, 0.35f, 0.9f, 1.0f });
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, v4{ 0.1f, 0.25f, 0.8f, 1.0f });
      if (ImGui::Button("Z", buttonSize))
        v.z = reset_value.z;
      ImGui::PopStyleColor(3);

      ImGui::SameLine();
      ImGui::DragFloat("##Z", &v.z, 0.1f, 0.0f, 0.0f, "%.2f");
      ImGui::PopItemWidth();

      ImGui::PopStyleVar();

      ImGui::Columns(1);

      ImGui::PopID();
    }

    void ShowDataToUI(const std::string &label, v4 &v, const v2 &reset_value) {
      ImGui::Text("%s", label.c_str());
      ImGui::SameLine();
      ImGui::InputFloat4(std::string("##v4" + label).c_str(), &v.x);
    }

  } // namespace axl::serializable

} // namespace axl
