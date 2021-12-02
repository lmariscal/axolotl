#pragma once

#include <axolotl/types.hh>
#include <axolotl/window.hh>
#include <axolotl/terminal.hh>

namespace axl {

  struct DockSpaceData {
    TerminalData *terminal;
    bool fullscreen = false;
    bool ratio_locked = false;

    bool show_world_editor = true;
    bool show_terminal = true;
    bool show_inspector = true;
    bool show_hierarchy = true;
    bool show_renderer = true;
    bool show_imgui_demo = false;
  };

  class DockSpace {
   public:
    void Draw(Window &window);

    DockSpaceData data;
    u32 dock_left;
    u32 dock_right;
    u32 dock_bottom;
    u32 dock_top;
    u32 dock_top_right;
    u32 dock_top_left;
    u32 dock_left_bottom;
    bool first_iteration = true;
  };

} // namespace axl
