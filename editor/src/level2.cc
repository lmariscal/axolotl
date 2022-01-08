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

namespace axl {

  void AILevel::AddCoin(const v2i &pos, const std::vector<std::string> &shader_paths) {
    Ento coin_ento = CreateEntity();
    coin_ento.Tag().value = "Coin";
    coin_ento.AddComponent<OBBCollider>(coin_ento.Transform().GetPosition(), v3(1.0f));
    RigidBody &coin_rb = coin_ento.AddComponent<RigidBody>(0.0);
    coin_rb.is_trigger = true;
    Transform &wall_transform = coin_ento.Transform();
    wall_transform.SetPosition(v3(pos.x, 2.5f, pos.y));
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
      std::fill(_maze[x].begin(), _maze[x].end(), false);
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
          AddCoin({ x * 2, z * 2 }, shader_paths);
        } else if (c == ' ') {
          if (first_empty_pos == v2(0))
            first_empty_pos = v2(x, z);
          last_empty_pos = v2(x, z);
        }
        x++;
      }
      z++;
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
    enemy_rb.is_trigger = true;
    Transform &enemy_transform = _enemy_ento.Transform();
    enemy_transform.SetPosition(v3(last_empty_pos.x * 2, 2, last_empty_pos.y * 2));
    enemy_transform.SetScale(v3(1.0f, 1.0f, 1.0f));
    Model &enemy_model = _enemy_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Sphere.fbx", shader_paths);
    _enemy_ento.AddComponent<Light>(LightType::Point, v3(1.0f, 0.0f, 0.0f), 0.2f);

    TextureStore::ProcessQueue();
    file.close();
  }

  void AILevel::Init(Window &window) {
    Ento camera_ento = CreateEntity();
    camera_ento.Tag().value = "Camera";
    Camera &camera_component = camera_ento.AddComponent<Camera>();
    camera_component.SetAsActive();
    camera_ento.Transform().SetPosition(v3(26, 57, 17));
    camera_ento.Transform().SetRotationEuler(v3(89.592f, -87.991f, -149.384f));

    _show_instructions = _first_time;

    window.GetRenderer().SetShowGrid(false);

    GenerateMazeFromFile();

    window.GetRenderer().SetAmbientLight(Light(LightType::Ambient, v3(0.6f), 0.6f));
    window.GetRenderer().SetDirectionalLight(Light(LightType::Directional, v3(1.0f), 0.6f));
  }

  void AILevel::Update(Window &window, f64 delta) {
    if (_show_instructions || _show_menu)
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

    if (no_landing) {
      _time_not_grounded += delta;
      if (_time_not_grounded > 9 || _player_ento.Transform().GetPosition().y < -10.0f)
        _game_over = true;
    } else {
      _time_not_grounded = 0;
    }

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
        _target_distance += 3.0f;
      if (io.ButtonDown(pad, PadButton::RightBumper))
        _target_distance -= 3.0f;

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

      if (!other_rb.is_trigger)
        continue;

      RemoveEntity(Ento::FromComponent(other_rb));
    }

    // Coin Rotate

    _registry.each([&](entt::entity e) {
      Ento ento = FromHandle(e);
      if (ento.Tag().value == "Coin") {
        quat rot = quat(radians(v3(0.0f, 0.0f, 60.0f) * (f32)delta));
        ento.Transform().SetRotation(ento.Transform().GetRotation() * rot);
      }
    });
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
      "The goal is to find the Enemy and destroy them by bumping with them.\n"
      "Will you be able to find them? They will try to be hiding inside the maze\n\n"
      "To move around you can use the keyboard or a controller.\n"
      "WASD or the left stick to move.\n\n"
      "To move the camera you can use the right stick or the arrow keys.\n\n"
      "You can also jump by pressing SPACE or the A button on the controller.\n"
      "Double jump is available, but it only recharges when you land.\n\n"
      "Zoom in and out with 0 and 9 numbers or bumpers in your controller\n\n");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::Text("You can exit the level by opening the menu with ESC or the START button.");
    ImGui::Text(
      "Debug: In the real game the enemy would be destroyed and you would win.\n"
      "But in this debug mode it will keep respawning.");

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
    if (!_show_instructions)
      MenuLevel::ShowBackMenu(window, frame_size, frame_pos, _show_menu);
    else
      ShowInstructions(window, frame_size, frame_pos);
  }

  void AILevel::Focused(Window &window, bool state) {
    focused = state;
  }

} // namespace axl
