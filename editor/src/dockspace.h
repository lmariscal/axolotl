#pragma once

#include <axolotl/types.h>
#include <axolotl/window.h>
#include <axolotl/terminal.h>

namespace axl {

  struct DockSpaceData {
    TerminalData *terminal_data;
    bool *resize_world_editor;
  };

  class DockSpace {
   public:
    void Draw(Window &window, DockSpaceData &data);

   protected:
    bool _first_iteration = true;
  };

} // namespace axl
