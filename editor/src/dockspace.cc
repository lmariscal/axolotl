#include "dockspace.h"

#include <imgui.h>

namespace axl {

  void DockSpace::Draw(Window &window, DockSpaceData &data) {
    ImGuiWindowFlags main_dock_space_window_flags = ImGuiWindowFlags_None;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 0.0f));
    main_dock_space_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
                                ImGuiWindowFlags_NoBackground;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, v2(0.0f, 3.0f));
    ImGui::Begin("MainDockSpaceWindow", nullptr, main_dock_space_window_flags);
    ImGui::PopStyleVar(4);

    ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_None;
    dock_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
    // dock_flags |= ImGuiDockNodeFlags_AutoHideTabBar;
    v2i window_size = window.GetWindowFrameBufferSize();
    ImGuiID dock_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dock_id, v2(0, 0), dock_flags);

    if (_first_iteration) {
      ImGui::DockBuilderRemoveNode(dock_id);
      ImGui::DockBuilderAddNode(dock_id, main_dock_space_window_flags | ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dock_id, v2(window_size.x, window_size.y));

      auto dock_id_left = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.18f, nullptr, &dock_id);
      auto dock_id_right = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.22f, nullptr, &dock_id);
      auto dock_id_bottom = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Down, 0.33f, nullptr, &dock_id);

      ImGui::DockBuilderDockWindow("World Editor", dock_id);
      ImGui::DockBuilderDockWindow("Entities", dock_id_left);
      ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
      ImGui::DockBuilderDockWindow("Terminal", dock_id_bottom);

      _first_iteration = false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, v2(0.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 3.0f));
    if (ImGui::BeginMenuBar()) {

      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Quit")) {
          data.terminal_data->quit_requested = true;
        }
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }
    ImGui::PopStyleVar(2);

    ImGui::End();
  }

} // namespace axl
