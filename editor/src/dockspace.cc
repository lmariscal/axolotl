#include "dockspace.hh"

#include <IconsFontAwesome5Pro.h>
#include <fstream>
#include <imgui.h>
#include <nfd.h>

namespace axl {

  void DockSpace::LoadProject() {
    data.terminal->scene_playing = false;
    data.terminal->scene_paused = false;

    nfdchar_t *out_path = nullptr;
    nfdresult_t result =
      NFD_PickFolder(data.project_path.empty() ? nullptr : data.project_path.string().c_str(), &out_path);

    if (result == NFD_OKAY) {
      std::string path;
      i32 len = std::strlen(out_path);
      path.resize(len);
      std::strncpy(path.data(), out_path, len);

      data.project_path = std::filesystem::path(path);

      if (!std::filesystem::is_directory(data.project_path)) {
        data.project_path = "";
        log::error("Project path does not exist");
        return;
      }
      if (!std::filesystem::exists(data.project_path / "project.json")) {
        log::error("Project path does not contain `project.json`");
        return;
      }

      std::ifstream file(data.project_path / "project.json");
      if (!file.is_open()) {
        data.project_path = "";
        log::error("Failed to open project file `project.json`");
        return;
      }
      json j = json::parse(file, nullptr, false);
      data.scene->Deserialize(j);
      file.close();

      NFD_Free(out_path);
    } else if (result == NFD_CANCEL) {
    } else {
      log::error("Error: {}", NFD_GetError());
    }
  }

  void DockSpace::SaveProject() {
    if (data.project_path.empty()) {
      return;
    }

    std::ofstream file(data.project_path / "project.json");
    json j = data.scene->Serialize();
    file << j.dump(4) << std::endl;
    file.close();
  }

  bool DockSpace::SelectProjectPath() {
    nfdchar_t *out_path = nullptr;
    nfdresult_t result =
      NFD_PickFolder(data.project_path.empty() ? nullptr : data.project_path.string().c_str(), &out_path);

    if (result == NFD_OKAY) {
      data.project_path = out_path;

      NFD_Free(out_path);
      return true;
    } else if (result == NFD_CANCEL) {
    } else {
      data.project_path = "";
    }
    return false;
  }

  void DockSpace::Draw(Window &window) {
    ImGuiWindowFlags main_dock_space_window_flags = ImGuiWindowFlags_None;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 0.0f));
    main_dock_space_window_flags |=
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar |
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
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
      ImGui::DockBuilderSplitNode(dock_top_right, ImGuiDir_Up, 0.5f, &dock_top_right_top, &dock_top_right_bottom);

      ImGui::DockBuilderDockWindow("World Editor", dock_top_left);
      ImGui::DockBuilderDockWindow("Entities", dock_left);
      ImGui::DockBuilderDockWindow("Inspector", dock_left_bottom);
      ImGui::DockBuilderDockWindow("Terminal", dock_bottom);
      ImGui::DockBuilderDockWindow("Performance", dock_top_right_top);
      ImGui::DockBuilderDockWindow("Renderer", dock_top_right_bottom);

      first_iteration = false;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, v2(3.0f, 3.0f));
    if (ImGui::BeginMenuBar()) {

      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem(ICON_FA_FILE_PLUS " New", "C-n")) {
          // TODO
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem(ICON_FA_FILE_SEARCH " Open", "C-o")) {
          LoadProject();
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem(ICON_FA_SAVE " Save", "C-s")) {
          if (data.project_path.empty()) {
            SelectProjectPath();
          }
          SaveProject();
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem(ICON_FA_FILE_EXPORT " Save As", "C-S-s")) {
          if (SelectProjectPath())
            SaveProject();
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_FA_DOOR_OPEN " Quit", "C-S-q")) {
          data.terminal->quit_requested = true;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Edit")) {
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Aspect Ratio")) {
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem("Full Screen", "F11", data.fullscreen)) {
          data.fullscreen = !data.fullscreen;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem("Wireframe", "", data.show_wireframe)) {
          data.show_wireframe = !data.show_wireframe;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Window")) {
        if (ImGui::MenuItem("World Editor", nullptr, data.show_world_editor)) {
          data.show_world_editor = !data.show_world_editor;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem("Entities", nullptr, data.show_hierarchy)) {
          data.show_hierarchy = !data.show_hierarchy;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem("Inspector", nullptr, data.show_inspector)) {
          data.show_inspector = !data.show_inspector;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem("Renderer", nullptr, data.show_renderer)) {
          data.show_renderer = !data.show_renderer;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        if (ImGui::MenuItem("Terminal", nullptr, data.show_terminal)) {
          data.show_terminal = !data.show_terminal;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

        ImGui::Separator();

        if (ImGui::MenuItem("ImGui Demo", nullptr, data.show_imgui_demo)) {
          data.show_imgui_demo = !data.show_imgui_demo;
        }
        if (ImGui::IsItemHovered())
          ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

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
