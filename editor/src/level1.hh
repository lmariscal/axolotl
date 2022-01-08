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

    void ShowScoreMenu(Window &window, const v2 &frame_size, const v2 &frame_pos);
    void ShowInstructions(Window &window, const v2 &frame_size, const v2 &frame_pos);

   protected:
    bool _show_menu = false;
    i32 _score = 0;
    i32 _platform_n = 0;
    uuid _player_id;
    uuid _platform_id;
    uuid _elevator_id;
    json _world_data;

    f64 _time_not_grounded = 0.0;
    v2 _camera_position = { 0, 0.2f };
    f32 _target_distance = 30.0f;
    bool _double_jump = false;
    bool _game_over = false;
    bool _show_instructions = true;

    inline static bool _first_time = true;
  };

} // namespace axl
