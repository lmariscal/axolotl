#pragma once

#include <axolotl/scene.hh>
#include <axolotl/types.hh>

namespace axl {

  class AILevel: public Scene {
    virtual ~AILevel() { }
    void Init(Window &window) override;
    void Update(Window &window, f64 delta) override;
    void UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) override;
    void Focused(Window &window, bool state) override;

   protected:
    bool _show_menu = false;
  };

} // namespace axl
