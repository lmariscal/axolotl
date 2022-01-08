#include "level2.hh"

#include "menu.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/physics.hh>
#include <axolotl/renderer.hh>
#include <axolotl/window.hh>
#include <fstream>

namespace axl {

  void AILevel::Init(Window &window) { }

  void AILevel::Update(Window &window, f64 delta) { }

  void AILevel::UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    MenuLevel::ShowBackMenu(window, frame_size, frame_pos, _show_menu);
  }

  void AILevel::Focused(Window &window, bool state) { }

} // namespace axl
