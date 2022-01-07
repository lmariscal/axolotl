#pragma once

#include <axolotl/scene.hh>
#include <axolotl/types.hh>

namespace axl {

  class PhysicsLevel: public Scene {
   public:
    virtual ~PhysicsLevel() { }
    void Init(Window &window) override;
    void Update(Window &window, f64 delta) override;
    void UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) override;
    void Focused(Window &window, bool state) override;

   protected:
    bool _show_menu = false;
    i32 _score = 0;
    uuid _player_id;
    json _world_data;

    v2 _camera_position = { 0, 0 };
    f32 _target_distance = 15.0f;
    bool _double_jump = false;
  };

} // namespace axl
