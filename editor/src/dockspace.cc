#include "dockspace.h"

#include <imgui.h>

namespace axl {

  void DockSpace::Draw(Window &window) {
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
    dock_flags |= ImGuiDockNodeFlags_AutoHideTabBar;
    v2i window_size = window.GetWindowFrameBufferSize();
    ImGuiID dock_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dock_id, v2(0, 0), dock_flags);

    if (first_iteration) {
      // TODO Copy dock layout to properly restore it
      ImGui::DockBuilderRemoveNode(dock_id);
      ImGui::DockBuilderAddNode(dock_id, main_dock_space_window_flags | ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dock_id, v2(window_size.x, window_size.y));

      ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.21f, &dock_left, &dock_right);
      ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Down, 0.27f, &dock_bottom, &dock_top);

      ImGui::DockBuilderSplitNode(dock_left, ImGuiDir_Down, 0.5f, &dock_left_bottom, &dock_left);
      ImGui::DockBuilderSplitNode(dock_top, ImGuiDir_Right, 0.2f, &dock_top_right, &dock_top_left);

      ImGui::DockBuilderDockWindow("World Editor", dock_top_left);
      ImGui::DockBuilderDockWindow("Entities", dock_left);
      ImGui::DockBuilderDockWindow("Inspector", dock_left_bottom);
      ImGui::DockBuilderDockWindow("Terminal", dock_bottom);
      ImGui::DockBuilderDockWindow("Renderer", dock_top_right);

      first_iteration = false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, v2(6.0f, 3.0f));
    if (ImGui::BeginMenuBar()) {

      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Quit")) {
          data.terminal->quit_requested = true;
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Edit")) {
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Editor Aspect Ratio")) {
        }
        if (ImGui::MenuItem("Editor Full Screen", nullptr, data.fullscreen)) {
          data.fullscreen = !data.fullscreen;
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Window")) {
        if (ImGui::MenuItem("World Editor")) {
        }
        if (ImGui::MenuItem("Entities")) {
        }
        if (ImGui::MenuItem("Inspector")) {
        }
        if (ImGui::MenuItem("Terminal")) {
        }
        if (ImGui::MenuItem("ImGui Demo", nullptr, data.show_imgui_demo)) {
          data.show_imgui_demo = !data.show_imgui_demo;
        }
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }
    ImGui::PopStyleVar(1);

    ImGui::End();

    if (data.show_imgui_demo) {
      ImGui::ShowDemoWindow(&data.show_imgui_demo);
    }
  }

} // namespace axl
