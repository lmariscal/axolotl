#include "level2.hh"

#include "menu.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/model.hh>
#include <axolotl/physics.hh>
#include <axolotl/renderer.hh>
#include <axolotl/texture.hh>
#include <axolotl/window.hh>
#include <fstream>
#include <functional>

namespace axl {

  // Basic Logic of AI
  // 1. Are we being attacked? (Raycast detects player)
  //   a. If so, run away
  //   b. If not, find coins
  // 2. Do we know where a coin is?
  //  a. If so, go there
  //  b. If not, explore the map (Randomly, Raycast detects coin)
  //
  // Selector:
  //    Check if we are being attacked -> Returns Failure on no detection, and Success on detection of a player
  //        This in itself is a sequence:
  //          Check if we are being attacked -> Returns Failure on no detection, and Success on detection of a player
  //          Run away -> Returns Failure on no possible path, and Success on path found
  //            // Haha, so if we have no path and we are being attacked, we just go searching for coin
  //    Explore for coin -> Returns Failure on no detection, and Success on detection of a coin
  //        Inverter: // Need it since a state that stops a selector child will stop a selector parent
  //        This in itself is a selector:
  //            Know Where coin Is? -> Returns Failure on coin not memorized, and Success on coin memorized
  //            Explore for Coin -> Returns Failure on no possible path, and Success on path found
  //    Go towards coin -> Returns Failure on no path, and Success on path found
  //        Go towards coin -> Returns Failure on no path, and Success on path found
  //
  // If the root Selector fails, either we have explored the entire map and found no coin, or we are stuck

  constexpr v3 light_color_exploring = v3(0, 172, 193) / 255.0f;
  constexpr v3 light_color_coin_found = v3(255, 179, 0) / 255.0f;
  constexpr v3 light_color_run_away = v3(236, 64, 122) / 255.0f;

  v2i AILevel::ToMazeCoord(const v2 &pos) {
    v2i maze_pos;
    maze_pos.x = (i32)std::ceil(pos.x * 0.5f);
    maze_pos.y = (i32)std::ceil(pos.y * 0.5f);

    if (maze_pos.x >= _maze.size())
      maze_pos.x = _maze.size() - 1;
    if (maze_pos.y >= _maze[0].size())
      maze_pos.y = _maze[0].size() - 1;

    if (maze_pos.x < 0)
      maze_pos.x = 0;
    if (maze_pos.y < 0)
      maze_pos.y = 0;

    if (_maze[maze_pos.x][maze_pos.y]) {
      maze_pos.x = (i32)std::floor(pos.x * 0.5f);
      maze_pos.y = (i32)std::floor(pos.y * 0.5f);
    }

    if (maze_pos.x >= _maze.size())
      maze_pos.x = _maze.size() - 1;
    if (maze_pos.y >= _maze[0].size())
      maze_pos.y = _maze[0].size() - 1;

    if (maze_pos.x < 0)
      maze_pos.x = 0;
    if (maze_pos.y < 0)
      maze_pos.y = 0;

    return maze_pos;
  }

  void AILevel::ShowScoreMenu(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(frame_pos.x + (frame_size.x * 0.1f), frame_pos.y + (frame_size.y * 0.1f)),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(frame_size.x * 0.6f, frame_size.y * 0.8f), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));

    ImGui::Begin("Menu", nullptr, window_flags);

    ImGuiIO &io = ImGui::GetIO();
    ImGui::PushFont(io.Fonts->Fonts[1]);
    if (_coins_available <= 0)
      ImGui::Text("You Lost!");
    else
      ImGui::Text("You Won!");
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    if (ImGui::Button("Restart Level")) {
      Scene::SetActiveScene(new AILevel());
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    if (ImGui::Button("Back to Main Menu")) {
      Scene::SetActiveScene(new MenuLevel());
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::End();
    ImGui::PopStyleVar();
  }

  v2 AILevel::FindFurthestPos(const v2 &pos, const std::vector<std::vector<bool>> &map) {
    v2 furthest_pos = pos;
    f32 furthest_dist = 0.0f;
    for (i32 i = 0; i < map.size(); ++i) {
      for (i32 j = 0; j < map[i].size(); ++j) {
        if (map[i][j])
          continue;

        f32 dist = length(v2(i, j) - pos);
        if (dist > furthest_dist) {
          furthest_dist = dist;
          furthest_pos = v2(i, j);
        }
      }
    }

    return furthest_pos;
  }

  std::vector<PathNode> AILevel::FindPath(const PathNode &pos, const PathNode &target) {
    std::vector<PathNode> to_search;
    to_search.push_back(pos);
    std::vector<PathNode> processed;

    i32 i = 0;
    while (!to_search.empty()) {
      i++;
      PathNode current = to_search.back();
      for (PathNode &ts : to_search) {
        if (ts.F < current.F || (ts.F == current.F && ts.H < current.H))
          current = ts;
      }

      to_search.erase(std::find(to_search.begin(), to_search.end(), current));
      processed.push_back(current);

      if (current == target) {
        // log::debug("Found path in {} iterations", i);
        std::vector<PathNode> path;
        path.push_back(current);
        while (current.connection) {
          current = *current.connection;
          path.push_back(current);
        }
        return path;
      }

      std::vector<PathNode> neighbours = GetNeighbours(current);
      for (PathNode &n : neighbours) {
        if (!n.walkable)
          continue;
        if (std::find(processed.begin(), processed.end(), n) != processed.end())
          continue;

        bool pending_search = std::find(to_search.begin(), to_search.end(), n) != to_search.end();
        f32 cost = current.G + length(current.position - n.position);

        if (!pending_search || cost < n.G) {
          n.G = cost;
          n.connection = std::unique_ptr<PathNode>(new PathNode(current));

          if (!pending_search) {
            n.H = length(n.position - target.position);
            to_search.push_back(n);
          } else {

            for (PathNode &ts : to_search) {
              if (ts == n) {
                ts.G = cost;
                ts.connection = std::unique_ptr<PathNode>(new PathNode(current));
                continue;
              }
            }
          }
        }
      }
    }

    return {};
  }

  std::vector<PathNode> AILevel::GetNeighbours(const PathNode &pos) {
    std::vector<v2> neighbours;
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (i == 0 && j == 0)
          continue;

        v2 n = pos.position + v2(i, j);
        if (n.x >= 0 && n.x < _maze.size() && n.y >= 0 && n.y < _maze[0].size())
          neighbours.push_back(n);
      }
    }

    std::vector<PathNode> path_nodes;
    for (v2 &n : neighbours) {
      PathNode pn;
      pn.position = n;
      pn.G = length(pos.position - n);
      pn.walkable = !_maze[n.x][n.y];

      v2i last_known_player_position = ToMazeCoord({ _last_known_player_position.x, _last_known_player_position.z });

      if (n.x == last_known_player_position.x && n.y == last_known_player_position.y)
        pn.walkable = false;

      path_nodes.push_back(pn);
    }

    return path_nodes;
  }

  void AILevel::AddCoin(const v2i &pos, const std::vector<std::string> &shader_paths) {
    Ento coin_ento = CreateEntity();
    coin_ento.Tag().value = "Coin";
    coin_ento.AddComponent<OBBCollider>(coin_ento.Transform().GetPosition(), v3(1.0f));
    RigidBody &coin_rb = coin_ento.AddComponent<RigidBody>(0.0);
    coin_rb.is_trigger = true;
    Transform &wall_transform = coin_ento.Transform();
    wall_transform.SetPosition(v3(pos.x, 2.0f, pos.y));
    wall_transform.SetScale(v3(0.6f, 0.6f, 0.6f));
    wall_transform.SetRotationEuler(v3(90.0f, 0.0f, 0.0f));

    Model &wall_model = coin_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Torus.fbx", shader_paths);
    _coins_available++;
  }

  void AILevel::AddWall(const v2i &pos, const std::vector<std::string> &shader_paths) {
    // Greedy Meshing, anyone? No? Okay, let's do it the unoptimized way.
    // Or at least, you know, make it one mesh for the long walls.

    Ento wall_ento = CreateEntity();
    wall_ento.Tag().value = "Wall";
    wall_ento.AddComponent<OBBCollider>(wall_ento.Transform().GetPosition(), v3(1.0f));
    wall_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_transform = wall_ento.Transform();
    wall_transform.SetPosition(v3(pos.x, 4.0f, pos.y));
    wall_transform.SetScale(v3(1.0f, 3.0f, 1.0f));

    Model &wall_model = wall_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
  }

  void AILevel::GenerateMazeFromFile() {
    std::vector<std::string> shader_paths = { Axolotl::GetDistDir() + "res/shaders/testy.vert",
                                              Axolotl::GetDistDir() + "res/shaders/testy.frag" };

    std::ifstream file(Axolotl::GetDistDir() + "res/misc/maze.txt");
    if (!file.is_open()) {
      log::error("could not open maze file");
      return;
    }

    std::string line;
    std::getline(file, line);
    i32 width = std::stoi(line);
    std::getline(file, line);
    i32 height = std::stoi(line);

    _maze.resize(width);
    for (i32 x = 0; x < width; ++x) {
      _maze[x].resize(height);
      std::fill(_maze[x].begin(), _maze[x].end(), true);
    }

    v2 first_empty_pos = v2(0);
    v2 last_empty_pos = v2(0);

    i32 z = 0;
    while (std::getline(file, line)) {
      if (z == 0 || z == height - 1) {
        z++;
        continue;
      }

      i32 x = 0;
      for (char c : line) {
        if (x == 0 || x == width - 1) {
          x++;
          continue;
        }

        if (c == '#') {
          AddWall({ x * 2, z * 2 }, shader_paths);
          _maze[x][z] = true;
        } else if (c == '$') {
          _maze[x][z] = false;
          AddCoin({ x * 2, z * 2 }, shader_paths);
        } else if (c == ' ') {
          _maze[x][z] = false;
          if (first_empty_pos == v2(0))
            first_empty_pos = v2(x, z);
          last_empty_pos = v2(x, z);
        }
        x++;
      }
      z++;
    }

    _explored_maze.resize(width);
    for (i32 x = 0; x < width; ++x) {
      _explored_maze[x].resize(height);
      std::fill(_explored_maze[x].begin(), _explored_maze[x].end(), false);
      for (i32 y = 0; y < height; ++y) {
        if (_maze[x][y])
          _explored_maze[x][y] = true;
      }
    }

    Ento floor_ento = CreateEntity();
    floor_ento.Tag().value = "Floor";
    floor_ento.AddComponent<OBBCollider>(floor_ento.Transform().GetPosition(), v3(1.0f));
    floor_ento.AddComponent<RigidBody>(0.0);
    Transform &floor_transform = floor_ento.Transform();
    floor_transform.SetPosition(v3(width - 1, 0, height - 1));
    floor_transform.SetScale(v3(width, 1.0f, height));
    Model &wall_model = floor_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_b_ento = CreateEntity();
    wall_b_ento.Tag().value = "Wall B";
    wall_b_ento.AddComponent<OBBCollider>(wall_b_ento.Transform().GetPosition(), v3(1.0f));
    wall_b_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_b_transform = wall_b_ento.Transform();
    wall_b_transform.SetPosition(v3(width - 1, 4.0f, (height - 1) * 2.0f));
    wall_b_transform.SetScale(v3(width, 3.0f, 1.0f));
    Model &wall_b_model = wall_b_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_c_ento = CreateEntity();
    wall_c_ento.Tag().value = "Wall C";
    wall_c_ento.AddComponent<OBBCollider>(wall_c_ento.Transform().GetPosition(), v3(1.0f));
    wall_c_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_c_transform = wall_c_ento.Transform();
    wall_c_transform.SetPosition(v3((width - 1) * 2.0f, 4.0f, height - 1));
    wall_c_transform.SetScale(v3(1.0f, 3.0f, height));
    Model &wall_c_model = wall_c_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_a_ento = CreateEntity();
    wall_a_ento.Tag().value = "Wall A";
    wall_a_ento.AddComponent<OBBCollider>(wall_a_ento.Transform().GetPosition(), v3(1.0f));
    wall_a_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_a_transform = wall_a_ento.Transform();
    wall_a_transform.SetPosition(v3(0, 4.0f, height - 1));
    wall_a_transform.SetScale(v3(1.0f, 3.0f, height));
    Model &wall_a_model = wall_a_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_d_ento = CreateEntity();
    wall_d_ento.Tag().value = "Wall D";
    wall_d_ento.AddComponent<OBBCollider>(wall_d_ento.Transform().GetPosition(), v3(1.0f));
    wall_d_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_d_transform = wall_d_ento.Transform();
    wall_d_transform.SetPosition(v3(width - 1, 4.0f, 0));
    wall_d_transform.SetScale(v3(width, 3.0f, 1.0f));
    Model &wall_d_model = wall_d_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    _player_ento = CreateEntity();
    _player_ento.Tag().value = "Player";
    _player_ento.AddComponent<SphereCollider>(_player_ento.Transform().GetPosition(), 1.0);
    _player_ento.AddComponent<RigidBody>(1.0);
    Transform &player_transform = _player_ento.Transform();
    player_transform.SetPosition(v3(first_empty_pos.x * 2, 2, first_empty_pos.y * 2));
    player_transform.SetScale(v3(1.0f, 1.0f, 1.0f));
    Model &player_model = _player_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Sphere.fbx", shader_paths);

    _enemy_ento = CreateEntity();
    _enemy_ento.Tag().value = "Enemy";
    _enemy_ento.AddComponent<SphereCollider>(_enemy_ento.Transform().GetPosition(), 1.0);
    RigidBody &enemy_rb = _enemy_ento.AddComponent<RigidBody>(0.0);
    Transform &enemy_transform = _enemy_ento.Transform();
    enemy_transform.SetPosition(v3(last_empty_pos.x * 2, 2, last_empty_pos.y * 2));
    enemy_transform.SetScale(v3(1.0f, 1.0f, 1.0f));
    Model &enemy_model = _enemy_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Sphere.fbx", shader_paths);
    _enemy_ento.AddComponent<Light>(LightType::Point, light_color_exploring, 0.2f);

    TextureStore::ProcessQueue();
    file.close();
  }

  void AILevel::SetupBehaviour() {
    _behaviour_root = std::unique_ptr<BehaviourNodeWithChildren>(new BehaviourSelector("Behaviour Root"));

    BehaviourNodeWithChildren *being_attacked_sequence = static_cast<BehaviourNodeWithChildren *>(
      _behaviour_root->AddChild(new BehaviourSequence("Being Attacked Sequence")));

    BehaviourInverter *explore_map_selector_inv = static_cast<BehaviourInverter *>(
      _behaviour_root->AddChild(new BehaviourInverter("Explore Map Selector Inverter")));
    BehaviourNodeWithChildren *explore_map_selector = static_cast<BehaviourNodeWithChildren *>(
      explore_map_selector_inv->SetChild(new BehaviourSelector("Explore Map Selector")));

    _behaviour_root->AddChild(
      new BehaviourAction("Go Towards Coin", [this](f32 step, BehaviourState state) -> BehaviourState {
        if (_known_coins.empty())
          return BehaviourState::Failed;

        // log::debug("Go Towards Coin");
        Light &light = _enemy_ento.GetComponent<Light>();
        light.SetColor(light_color_coin_found);

        v2 enemy_pos =
          ToMazeCoord({ _enemy_ento.Transform().GetPosition().x, _enemy_ento.Transform().GetPosition().z });

        std::vector<PathNode> path = FindPath(PathNode(enemy_pos), PathNode(_known_coins.back()));
        if (path.empty()) {
          f32 dist = length(enemy_pos - _known_coins.back());
          // log::debug("Distance to coin: {}", dist);
          if (dist < 2) {
            _next_enemy_position = _known_coins.back();
            return BehaviourState::Succeeded;
          }

          // log::debug("No path found for coin");
          return BehaviourState::Failed;
        }

        PathNode *next_node = &path.rbegin()[0];
        if (path.size() > 1) {
          next_node = &path.rbegin()[1];
        }
        if (path.size() > 1) {
          v3 next_pos(next_node->position.x * 2.0f,
                      _enemy_ento.Transform().GetPosition().y,
                      next_node->position.y * 2.0f);
          if (length(next_pos - _enemy_ento.Transform().GetPosition()) <= 1.0f)
            next_node = &path.rbegin()[2];
        }
        _next_enemy_position = next_node->position;

        return BehaviourState::Succeeded;
      }));

#pragma region Are we being attacked ?
    being_attacked_sequence->AddChild(
      new BehaviourAction("Check For Player", [this](f32 step, BehaviourState state) -> BehaviourState {
        // log::debug("Checking for player, player in range: {}", _player_in_range);
        if (this->_player_in_range)
          this->_time_since_saw_player = 10.0f;

        if (_time_since_saw_player > 0.0f) {
          this->_player_in_range = true;
          this->_time_since_saw_player -= Window::GetCurrentWindow()->GetDeltaTime();
        }
        return this->_player_in_range ? BehaviourState::Succeeded : BehaviourState::Failed;
      }));

    being_attacked_sequence->AddChild(
      new BehaviourAction("Run Away", [this](f32 step, BehaviourState state) -> BehaviourState {
        Light &light = _enemy_ento.GetComponent<Light>();
        light.SetColor(light_color_run_away);

        // log::debug("Running away from player");
        v2i player_pos = ToMazeCoord({ _last_known_player_position.x, _last_known_player_position.z });
        v2i enemy_pos =
          ToMazeCoord({ _enemy_ento.Transform().GetPosition().x, _enemy_ento.Transform().GetPosition().z });

        v2 furthest_pos = FindFurthestPos(player_pos, _maze);

        // log::debug("Furthest pos is {}", to_string(furthest_pos));
        // log::debug("Enemy pos: {}", to_string(enemy_pos));
        bool furthest_is_wall = _maze[furthest_pos.x][furthest_pos.y];
        // log::debug("Furthest is wall: {}", furthest_is_wall);

        std::vector<PathNode> path = FindPath(PathNode(enemy_pos), PathNode(furthest_pos));
        if (path.empty()) {
          // log::debug("No path found to run away");
          return BehaviourState::Failed;
        }

        PathNode *next_node = &path.rbegin()[0];
        if (path.size() > 1) {
          next_node = &path.rbegin()[1];
        }
        if (path.size() > 1) {
          v3 next_pos(next_node->position.x * 2.0f,
                      _enemy_ento.Transform().GetPosition().y,
                      next_node->position.y * 2.0f);
          if (length(next_pos - _enemy_ento.Transform().GetPosition()) <= 1.0f)
            next_node = &path.rbegin()[2];
        }
        _next_enemy_position = next_node->position;

        return BehaviourState::Succeeded;
      }));
#pragma endregion
#pragma region Explore Map

    explore_map_selector->AddChild(
      new BehaviourAction("Know Where Coin Is?", [this](f32 step, BehaviourState state) -> BehaviourState {
        // log::debug("Know Where Coin Is?");
        if (_known_coins.empty())
          return BehaviourState::Failed;

        // log::debug("Coin is at {}", to_string(_known_coins.back()));
        return BehaviourState::Succeeded;
      }));

    explore_map_selector->AddChild(
      new BehaviourAction("Explore For Coin", [this](f32 step, BehaviourState state) -> BehaviourState {
        Light &light = _enemy_ento.GetComponent<Light>();
        light.SetColor(light_color_exploring);

        v2 enemy_pos =
          ToMazeCoord({ _enemy_ento.Transform().GetPosition().x, _enemy_ento.Transform().GetPosition().z });
        f32 dist = length(enemy_pos - _to_explore);

        if (_to_explore == v2(0) || _explored_maze[_to_explore.x][_to_explore.y] || dist < 2.0f) {
          _explored_maze[_to_explore.x][_to_explore.y] = true;
          v2 furthest_pos = FindFurthestPos(enemy_pos, _explored_maze);
          if (furthest_pos == enemy_pos) {
            return BehaviourState::Failed;
          }

          _to_explore = furthest_pos;
        }

        std::vector<PathNode> path = FindPath(PathNode(enemy_pos), PathNode(_to_explore));
        if (path.empty()) {
          _explored_maze[_to_explore.x][_to_explore.y] = true;
          return BehaviourState::Failed;
        }

        PathNode *next_node = &path.rbegin()[0];
        if (path.size() > 1) {
          next_node = &path.rbegin()[1];
        }
        if (path.size() > 2) {
          v3 next_pos(next_node->position.x * 2.0f,
                      _enemy_ento.Transform().GetPosition().y,
                      next_node->position.y * 2.0f);
          if (length(next_pos - _enemy_ento.Transform().GetPosition()) <= 1.0f)
            next_node = &path.rbegin()[2];
        }

        _next_enemy_position = next_node->position;

        return BehaviourState::Succeeded;
      }));
#pragma endregion
  }

  void AILevel::Init(Window &window) {
    Ento camera_ento = CreateEntity();
    camera_ento.Tag().value = "Camera";
    Camera &camera_component = camera_ento.AddComponent<Camera>();
    camera_component.SetAsActive();
    camera_ento.Transform().SetPosition(v3(26, 57, 17));
    camera_ento.Transform().SetRotationEuler(v3(89.592f, -87.991f, -149.384f));

    _show_instructions = _first_time;
    _next_enemy_position = v2(0, 0);
    _to_explore = v2(0, 0);

    window.GetRenderer().SetShowGrid(false);

    GenerateMazeFromFile();
    SetupBehaviour();

    // v2i player_pos =
    //   ceil(v2(_player_ento.Transform().GetPosition().x, _player_ento.Transform().GetPosition().z) / 2.0f);
    // v2i enemy_pos = ceil(v2(_enemy_ento.Transform().GetPosition().x, _enemy_ento.Transform().GetPosition().z)
    // / 2.0f);

    // log::debug("Player pos: {}", to_string(player_pos));
    // log::debug("Enemy pos: {}", to_string(enemy_pos));

    // for (i32 i = 0; i < _maze.size(); ++i) {
    //   std::stringstream ss;
    //   for (i32 j = 0; j < _maze[i].size(); ++j) {
    //     if (player_pos.x == i && player_pos.y == j) {
    //       if (_maze[i][j])
    //         ss << "M";
    //       else
    //         ss << "P";
    //     } else if (enemy_pos.x == i && enemy_pos.y == j) {
    //       if (_maze[i][j])
    //         ss << "S";
    //       else
    //         ss << "E";
    //     } else {
    //       ss << _maze[i][j];
    //     }
    //   }
    //   log::debug("{}", ss.str());
    // }

    // std::vector<PathNode> path = FindPath(PathNode(enemy_pos), PathNode(v2(player_pos)));
    // log::debug("Path found: {}", path.size());
    // std::vector<std::string> shader_paths = { Axolotl::GetDistDir() + "res/shaders/testy.vert",
    //                                           Axolotl::GetDistDir() + "res/shaders/testy.frag" };
    // for (PathNode &node : path) {
    //   AddCoin(node.position * 2.0f, shader_paths);
    //   log::debug("{}", to_string(node.position));
    // }

    for (i32 i = 0; i < _ai_look_lines.size(); ++i)
      _ai_look_lines[i] = std::unique_ptr<LinePrimitive>(
        new LinePrimitive(_enemy_ento.Transform().GetPosition(), _enemy_ento.Transform().GetPosition()));

    window.GetRenderer().SetAmbientLight(Light(LightType::Ambient, v3(0.6f), 0.6f));
    window.GetRenderer().SetDirectionalLight(Light(LightType::Directional, v3(1.0f), 0.6f));
  }

  void AILevel::Update(Window &window, f64 delta) {
    if (_show_instructions || _show_menu)
      return;

    if (_game_over)
      return;

    IOManager &io = window.GetIOManager();

    Ento camera_ento = Camera::GetActiveCameraEnto();
    Camera &camera = camera_ento.GetComponent<Camera>();

    if (!camera_ento)
      return;

    const Transform &player_transform = _player_ento.Transform();

    v3 movement { sin(_camera_position.x), _camera_position.y, cos(_camera_position.x) };
    v3 pos = normalize(movement) * _target_distance;
    pos += player_transform.GetPosition();

    v3 front = normalize(player_transform.GetPosition() - pos);
    v3 right = normalize(cross({ 0.0f, 1.0f, 0.0f }, front));

    m4 view = lookAt(pos, player_transform.GetPosition(), { 0.0f, 1.0f, 0.0f });

    camera.SetCustomViewMatrix(view);

    front.y = 0.0f;
    front = normalize(front);
    right.y = 0.0f;
    right = normalize(right);

    RigidBody &rb = _player_ento.GetComponent<RigidBody>();

    bool no_landing = true;
    bool grounded = false;
    v3 start_ray = player_transform.GetPosition();
    SphereCollider &sc = _player_ento.GetComponent<SphereCollider>();
    Ray ray(start_ray, { 0.0f, -1.0f, 0.0f });

    _registry.view<OBBCollider>().each([&](auto entity, OBBCollider &obb) {
      f32 dist = obb.RayInside(ray);
      if (dist > 0.0f) {
        no_landing = false;

        v3 closest = obb.ClosestPoint(sc.position);
        if (length2(closest - sc.position) <= sc.radius) {
          grounded = true;
        } else {
          // LinePrimitive lp(start_ray, closest, Color(0.0f, 1.0f, 0));
          // window.GetRenderer().AddLine(lp);
        }
        return;
      }
    });

    constexpr f32 MOVEMENT_SPEED = 6.0f;
    constexpr f32 ROTATION_SPEED = 2.0f;

    // More realistic to move this way, but funny to see the player move in the air
    // if (grounded) {
    if (io.KeyDown(Key::W))
      rb.AddLinearImpulse(front * (f32)delta * MOVEMENT_SPEED);
    if (io.KeyDown(Key::S))
      rb.AddLinearImpulse(-front * (f32)delta * MOVEMENT_SPEED);
    if (io.KeyDown(Key::A))
      rb.AddLinearImpulse(right * (f32)delta * MOVEMENT_SPEED);
    if (io.KeyDown(Key::D))
      rb.AddLinearImpulse(-right * (f32)delta * MOVEMENT_SPEED);

    if (grounded) {
      _double_jump = false;
    }

    if (io.KeyDown(Key::N9) || io.KeyDown(Key::KPSubtract))
      _target_distance += 3.0f * delta;
    if (io.KeyDown(Key::N0) || io.KeyDown(Key::KPAdd))
      _target_distance -= 3.0f * delta;

    if (io.KeyDown(Key::Left))
      _camera_position.x += ROTATION_SPEED * delta;
    if (io.KeyDown(Key::Right))
      _camera_position.x -= ROTATION_SPEED * delta;
    if (io.KeyDown(Key::Up))
      _camera_position.y += ROTATION_SPEED * delta;
    if (io.KeyDown(Key::Down))
      _camera_position.y -= ROTATION_SPEED * delta;

    if (io.KeyTriggered(Key::Space) && (grounded || !_double_jump)) {
      if (!_double_jump && !grounded)
        _double_jump = true;
      if (grounded)
        _player_ento.Transform().SetPosition(_player_ento.Transform().GetPosition() + v3 { 0.0f, 0.2f, 0.0f });
      rb.AddLinearImpulse({ 0.0f, 15.0f, 0.0f });
    }

    Pad pad = Pad::Last;
    for (i32 p = 0; p < (i32)Pad::Last; ++p) {
      if (io.PadPresent((Pad)p)) {
        pad = (Pad)p;
        break;
      }
    }

    if (pad != Pad::Last) {
      if (io.ButtonDown(pad, PadButton::LeftBumper))
        _target_distance += 20.0f * delta;
      if (io.ButtonDown(pad, PadButton::RightBumper))
        _target_distance -= 20.0f * delta;

      constexpr f32 AXIS_DEAD_ZONE = 0.3f;

      // if (grounded) {
      v2 left = { io.GetAxis(pad, JoyStick::LeftX), io.GetAxis(pad, JoyStick::LeftY) };

      if (left.x < AXIS_DEAD_ZONE && left.x > -AXIS_DEAD_ZONE)
        left.x = 0.0f;
      if (left.y < AXIS_DEAD_ZONE && left.y > -AXIS_DEAD_ZONE)
        left.y = 0.0f;

      v3 front_impulse = front * -left.y;
      v3 right_impulse = right * -left.x;
      rb.AddLinearImpulse(front_impulse * MOVEMENT_SPEED * (f32)delta);
      rb.AddLinearImpulse(right_impulse * MOVEMENT_SPEED * (f32)delta);
      // }
      if (io.ButtonTriggered(pad, PadButton::A) && (grounded || !_double_jump)) {
        if (!_double_jump && !grounded)
          _double_jump = true;
        if (grounded)
          _player_ento.Transform().SetPosition(_player_ento.Transform().GetPosition() + v3 { 0.0f, 0.1f, 0.0f });
        rb.AddLinearImpulse({ 0.0f, 15.0f, 0.0f });
      }

      // --------------------------------------------------------------------------------

      v2 right = { io.GetAxis(pad, JoyStick::RightX), io.GetAxis(pad, JoyStick::RightY) };

      if (right.x < AXIS_DEAD_ZONE && right.x > -AXIS_DEAD_ZONE)
        right.x = 0.0f;
      if (right.y < AXIS_DEAD_ZONE && right.y > -AXIS_DEAD_ZONE)
        right.y = 0.0f;

      _camera_position.x -= right.x * ROTATION_SPEED * (f32)delta;
      _camera_position.y -= right.y * ROTATION_SPEED * (f32)delta;
    }

    for (Ento &c : rb.colliding_with) {
      if (!c.HasComponent<RigidBody>())
        continue;

      RigidBody &other_rb = c.GetComponent<RigidBody>();

      if (c.Tag().value != "Enemy")
        continue;

      // This would make you win, but let's keep respawning the enemy
      // RemoveEntity(Ento::FromComponent(other_rb));

      v2i player_pos = ToMazeCoord({ player_transform.GetPosition().x, player_transform.GetPosition().z });

      v2 furthest = FindFurthestPos(player_pos, _maze);
      _enemy_ento.Transform().SetPosition(
        v3(furthest.x * 2.0f, _enemy_ento.Transform().GetPosition().y, furthest.y * 2.0f));
      _time_since_saw_player = 0.0f;

      log::info("Enemy respawned at {}, current enemy pos {}",
                to_string(furthest),
                to_string(_enemy_ento.Transform().GetPosition()));
    }

    // Coin Rotate

    _registry.each([&](entt::entity e) {
      Ento ento = FromHandle(e);
      if (ento.Tag().value == "Coin") {
        quat rot = quat(radians(v3(0.0f, 0.0f, 60.0f) * (f32)delta));
        ento.Transform().SetRotation(ento.Transform().GetRotation() * rot);
      }
    });

    // Enemy Behaviour

    constexpr std::array<v3, 16> check_axis = {
      v3(1.0f, 0.0f, 0.0f),   v3(-1.0f, 0.0f, 0.0f),   v3(0.0f, 0.0f, 1.0f),    v3(0.0f, 0.0f, -1.0f),
      v3(0.5f, 0.0f, 0.5f),   v3(-0.5f, 0.0f, 0.5f),   v3(0.5f, 0.0f, -0.5f),   v3(-0.5f, 0.0f, -0.5f),
      v3(0.25f, 0.0f, 0.25f), v3(-0.25f, 0.0f, 0.25f), v3(0.25f, 0.0f, -0.25f), v3(-0.25f, 0.0f, -0.25f),
      v3(0.15f, 0.0f, 0.15f), v3(-0.15f, 0.0f, 0.15f), v3(0.15f, 0.0f, -0.15f), v3(-0.15f, 0.0f, -0.15f),
    };

    _player_in_range = false;
    for (i32 i = 0; i < (i32)check_axis.size(); ++i) {
      const v3 &axis = check_axis[i];
      Ray ray(_enemy_ento.Transform().GetPosition(), axis);
      Ento min_dist_ento;
      f32 min_dist = std::numeric_limits<f32>::max();

      _registry.each([&](entt::entity e) {
        Ento other_ento = FromHandle(e);
        if (_enemy_ento == other_ento)
          return;
        if (other_ento.HasComponent<RigidBody>())
          RigidBody &other_rb = other_ento.GetComponent<RigidBody>();

        if (other_ento.HasComponent<SphereCollider>()) {
          SphereCollider &other_collider = other_ento.GetComponent<SphereCollider>();
          f32 dist = ray.SphereInside(other_collider);
          if (dist > 0.0f && dist < min_dist) {
            min_dist = dist;
            min_dist_ento = other_ento;
          }
        } else if (other_ento.HasComponent<OBBCollider>()) {
          OBBCollider &other_collider = other_ento.GetComponent<OBBCollider>();
          f32 dist = ray.OBBInside(other_collider);
          if (dist > 0.0f && dist < min_dist) {
            min_dist = dist;
            min_dist_ento = other_ento;

            if (other_ento.Tag().value == "Coin") {
              v2 coin_pos =
                ToMazeCoord({ other_ento.Transform().GetPosition().x, other_ento.Transform().GetPosition().z });
              if (std::find(_known_coins.begin(), _known_coins.end(), coin_pos) == _known_coins.end())
                _known_coins.push_back(coin_pos);
            }
          }
        }
      });

      if (!min_dist_ento)
        continue;

      if (min_dist_ento.Tag().value == "Player") {
        _last_known_player_position = min_dist_ento.Transform().GetPosition();
        _player_in_range = true;
        // log::debug("Player position: {}", to_string(_last_known_player_position));
      }

      _ai_look_lines[i]->SetPos(_enemy_ento.Transform().GetPosition(),
                                _enemy_ento.Transform().GetPosition() + axis * min_dist,
                                _player_in_range ? Color(light_color_exploring) : Color());
      window.GetRenderer().AddLine(_ai_look_lines[i].get());
    }

    _behaviour_root->Execute(delta);
    RigidBody &enemy_rb = _enemy_ento.GetComponent<RigidBody>();

    if (_next_enemy_position != v2(0.0f)) {
      v3 next_pos(
        v3(_next_enemy_position.x * 2.0f, _enemy_ento.Transform().GetPosition().y, _next_enemy_position.y * 2.0f));

      // TODO: Movement should be smooth
      // log::debug("Enemy moving to {}", to_string(next_pos));
      f32 enemy_speed = 1.0f * delta;
      _enemy_ento.Transform().SetPosition((_enemy_ento.Transform().GetPosition() * (1.0f - enemy_speed)) +
                                          (next_pos * enemy_speed));

      v2i enemy_pos = ToMazeCoord({ _enemy_ento.Transform().GetPosition().x, _enemy_ento.Transform().GetPosition().z });

      _explored_maze[(i32)enemy_pos.x][(i32)enemy_pos.y] = true;

      _next_enemy_position = v2(0.0f);
    }

    for (Ento &c : enemy_rb.colliding_with) {
      if (!_registry.valid(c))
        continue;
      if (!c.HasComponent<RigidBody>())
        continue;

      RigidBody &other_rb = c.GetComponent<RigidBody>();

      if (!other_rb.is_trigger)
        continue;

      if (c.Tag().value == "Coin") {
        _coins_available--;

        v2 coin_pos = ToMazeCoord({ c.Transform().GetPosition().x, c.Transform().GetPosition().z });
        // Coins are static, so we can just "guess" the correct position
        // log::debug("Coin collected at {}", to_string(coin_pos));
        _known_coins.erase(std::remove(_known_coins.begin(), _known_coins.end(), coin_pos), _known_coins.end());

        RemoveEntity(c);

        if (_coins_available <= 0) {
          _game_over = true;
        }
      }
    }
  }

  void AILevel::ShowInstructions(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(frame_pos.x + (frame_size.x * 0.1f), frame_pos.y + (frame_size.y * 0.1f)),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(frame_size.x * 0.6f, frame_size.y * 0.8f), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));

    ImGui::Begin("Menu", nullptr, window_flags);

    ImGuiIO &io = ImGui::GetIO();
    ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::Text("AI Level");
    ImGui::PopFont();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::Text(
      "While the enemy tries to find the coins scaterred around the map, you have to hunt them down!\n\n"
      "To move around you can use the keyboard or a controller.\n"
      "WASD or the left stick to move.\n\n"
      "To move the camera you can use the right stick or the arrow keys.\n\n"
      "You can also jump by pressing SPACE or the A button on the controller.\n"
      "Double jump is available, but it only recharges when you land.\n\n"
      "Zoom in and out with 0 and 9 numbers or bumpers in your controller.");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::Text("You can exit the level by opening the menu with ESC or the START button.");
    ImGui::Text(
      "Debug: In the real game the enemy would be destroyed and you would win.\n"
      "But in this debug mode it will keep respawning. And you will eventually lose, haha!");

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    if (ImGui::Button("Close Instructions")) {
      _show_instructions = false;
      _first_time = false;
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void AILevel::UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    if (_show_instructions)
      ShowInstructions(window, frame_size, frame_pos);
    else if (_game_over)
      ShowScoreMenu(window, frame_size, frame_pos);
    else
      MenuLevel::ShowBackMenu(window, frame_size, frame_pos, _show_menu);
  }

  void AILevel::Focused(Window &window, bool state) {
    focused = state;
  }

} // namespace axl
