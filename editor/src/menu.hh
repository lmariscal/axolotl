#pragma once

#include <axolotl/scene.hh>
#include <axolotl/types.hh>

namespace axl {

  class MenuLevel: public Scene {
   public:
    virtual ~MenuLevel() = default;
    void Init(Window &window) override;
    void Update(Window &window, f64 delta) override;
    void UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) override;
    void Focused(Window &window, bool state) override;

    static void ShowBackMenu(Window &window, const v2 &frame_size, const v2 &frame_pos, bool &show_menu);
  };

} // namespace axl
