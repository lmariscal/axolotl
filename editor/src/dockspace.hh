#pragma once

#include <axolotl/scene.hh>
#include <axolotl/terminal.hh>
#include <axolotl/types.hh>
#include <axolotl/window.hh>

namespace axl {

  class DockSpaceData {
   public:
    TerminalData *terminal;
    bool fullscreen = false;
    bool ratio_locked = false;

    bool show_wireframe = false;
    bool show_world_editor = true;
    bool show_terminal = true;
    bool show_inspector = true;
    bool show_hierarchy = true;
    bool show_renderer = true;
    bool show_guizmos = true;
    bool show_imgui_demo = false;
    bool show_performance = true;

    std::filesystem::path project_path = "";
    Scene *scene = nullptr;
  };

  class DockSpace {
   public:
    void Draw(Window &window);
    void LoadProject();
    void SaveProject();
    bool SelectProjectPath();

    DockSpaceData data;
    u32 dock_left;
    u32 dock_right;
    u32 dock_bottom;
    u32 dock_top;
    u32 dock_top_right;
    u32 dock_top_right_top;
    u32 dock_top_right_bottom;
    u32 dock_top_left;
    u32 dock_left_bottom;
    bool first_iteration = true;
  };

} // namespace axl
