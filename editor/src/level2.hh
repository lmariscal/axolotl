#pragma once

#include "behaviour.hh"

#include <axolotl/ento.hh>
#include <axolotl/line.hh>
#include <axolotl/scene.hh>
#include <axolotl/types.hh>

namespace axl {

  class PathNode {
   public:
    v2 position;
    f32 G;
    f32 H;
    f32 F;
    bool walkable;
    std::unique_ptr<PathNode> connection;

    PathNode(): G(0), H(0), F(0), connection(nullptr), walkable(false) { }
    PathNode(v2 position, f32 G = 0, f32 H = 0): position(position), G(G), H(H), F(G + H), walkable(false) { }
    PathNode(const PathNode &other):
      position(other.position),
      G(other.G),
      H(other.H),
      F(other.F),
      walkable(other.walkable) {
      if (other.connection)
        connection = std::make_unique<PathNode>(*other.connection);
      else
        connection.reset();
    }

    PathNode &operator=(const PathNode &other) {
      position = other.position;
      G = other.G;
      H = other.H;
      F = other.F;
      walkable = other.walkable;
      if (other.connection)
        connection = std::make_unique<PathNode>(*other.connection);
      else
        connection.reset();
      return *this;
    }

    bool operator==(const PathNode &other) const {
      return position == other.position;
    }
  };

  class AILevel: public Scene {
    virtual ~AILevel() { }
    void Init(Window &window) override;
    void Update(Window &window, f64 delta) override;
    void UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) override;
    void Focused(Window &window, bool state) override;

    void GenerateMazeFromFile();
    void AddWall(const v2i &pos, const std::vector<std::string> &shader_paths);
    void AddCoin(const v2i &pos, const std::vector<std::string> &shader_paths);
    void SetupBehaviour();
    void ShowInstructions(Window &window, const v2 &frame_size, const v2 &frame_pos);
    v2 FindFurthestPos(const v2 &pos, const std::vector<std::vector<bool>> &map);
    std::vector<PathNode> FindPath(const PathNode &pos, const PathNode &target);
    std::vector<PathNode> GetNeighbours(const PathNode &pos);

   protected:
    bool _show_menu = false;
    Ento _player_ento;
    Ento _enemy_ento;

    v2 _camera_position = { 0, 0.2f };
    v3 _last_known_player_position = v3(std::numeric_limits<f32>::min());
    i32 _coins_available = 0;
    f32 _target_distance = 30.0f;
    f64 _time_not_grounded = 0.0;
    bool _double_jump = false;
    bool _game_over = false;
    bool _player_in_range = false;

    std::array<std::unique_ptr<LinePrimitive>, 16> _ai_look_lines;
    std::unique_ptr<BehaviourNodeWithChildren> _behaviour_root;

    std::vector<std::vector<bool>> _maze;
    std::vector<std::vector<bool>> _explored_maze;
    std::vector<v2> _known_coins;
    v2 _next_enemy_position;
    bool _show_instructions = true;

    inline static bool _first_time = true;
  };

} // namespace axl
