#pragma once

#include <axolotl/types.h>
#include <axolotl/window.h>
#include <axolotl/terminal.h>

namespace axl {

  struct DockSpaceData {
    TerminalData *terminal;
    bool fullscreen = false;
    bool ratio_locked = false;
    bool show_imgui_demo = false;
  };

  class DockSpace {
   public:
    void Draw(Window &window);

    DockSpaceData data;

   protected:
    bool _first_iteration = true;
  };

} // namespace axl
