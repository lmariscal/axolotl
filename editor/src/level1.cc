#include "level1.hh"

#include "menu.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/physics.hh>
#include <axolotl/renderer.hh>
#include <axolotl/window.hh>
#include <fstream>

namespace axl {

  constexpr std::array<v3, 2> platform_pos = { v3(0.0f, 6.450f, -29.200f), v3(0.0f, 6.450f, -50.0f) };
  constexpr std::array<f32, 2> elevator_height = { 6.5f, 32.0f };

  void PhysicsLevel::ShowScoreMenu(Window &window, const v2 &frame_size, const v2 &frame_pos) {
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
    ImGui::Text("Score: %d", _score);
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    if (ImGui::Button("Restart Level")) {
      Scene::SetActiveScene(new PhysicsLevel());
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

  void PhysicsLevel::Init(Window &window) {
    std::fstream file(Axolotl::GetDistDir() + "res/misc/project.json");
    AXL_ASSERT_MESSAGE(file.is_open(), "could not open scene file for PhysicsLevel");

    _world_data = json::parse(file);
    Deserialize(_world_data);
    file.close();

    _player_id = *uuid::from_string("c63d2009-5657-4304-a9b7-8110d9f8b4c2");
    _platform_id = *uuid::from_string("ed2f5ded-2c14-456f-b426-43c5a45121cd");
    _elevator_id = *uuid::from_string("e5095876-2cb9-4ecd-abda-33d0ebd87143");
  }

  void PhysicsLevel::Update(Window &window, f64 delta) {
    IOManager &io = window.GetIOManager();

    Ento player_ento = FromID(_player_id);
    Ento camera_ento = Camera::GetActiveCameraEnto();
    Camera &camera = camera_ento.GetComponent<Camera>();

    if (!camera_ento)
      return;

    const Transform &player_transform = player_ento.Transform();

    v3 movement { sin(_camera_position.x), _camera_position.y, cos(_camera_position.x) };
    v3 pos = normalize(movement) * _target_distance;
    pos += player_transform.GetPosition();

    v3 front = normalize(player_transform.GetPosition() - pos);
    v3 right = normalize(cross({ 0.0f, 1.0f, 0.0f }, front));

    m4 view = lookAt(pos, player_transform.GetPosition(), { 0.0f, 1.0f, 0.0f });

    camera.SetCustomViewMatrix(view);

    RigidBody &rb = player_ento.GetComponent<RigidBody>();

    bool no_landing = true;
    bool grounded = false;
    v3 start_ray = player_transform.GetPosition();
    SphereCollider &sc = player_ento.GetComponent<SphereCollider>();
    Ray ray(start_ray, { 0.0f, -1.0f, 0.0f });

    _registry.view<OBBCollider>().each([&](auto entity, OBBCollider &obb) {
      f32 dist = obb.RayInside(ray);
      if (dist > 0.0f) {
        no_landing = false;

        v3 closest = obb.ClosestPoint(sc.position);
        if (length2(closest - sc.position) <= sc.radius) {
          grounded = true;
        } else {
          LinePrimitive lp(start_ray, closest, Color(0.0f, 1.0f, 0));
          window.GetRenderer().AddLine(lp);
        }
        return;
      }
    });

    if (no_landing) {
      _time_not_grounded += delta;
      if (_time_not_grounded > 9 || player_ento.Transform().GetPosition().y < -10.0f)
        _game_over = true;
    } else {
      _time_not_grounded = 0;
    }

    constexpr f32 MOVEMENT_SPEED = 6.0f;
    constexpr f32 ROTATION_SPEED = 2.0f;

    if (grounded) {
      if (io.KeyDown(Key::W))
        rb.AddLinearImpulse(front * (f32)delta * MOVEMENT_SPEED);
      if (io.KeyDown(Key::S))
        rb.AddLinearImpulse(-front * (f32)delta * MOVEMENT_SPEED);
      if (io.KeyDown(Key::A))
        rb.AddLinearImpulse(right * (f32)delta * MOVEMENT_SPEED);
      if (io.KeyDown(Key::D))
        rb.AddLinearImpulse(-right * (f32)delta * MOVEMENT_SPEED);

      _double_jump = false;
    }

    if (io.KeyDown(Key::Left))
      _camera_position.x -= ROTATION_SPEED * delta;
    if (io.KeyDown(Key::Right))
      _camera_position.x += ROTATION_SPEED * delta;
    if (io.KeyDown(Key::Up))
      _camera_position.y += ROTATION_SPEED * delta;
    if (io.KeyDown(Key::Down))
      _camera_position.y -= ROTATION_SPEED * delta;

    if (io.KeyTriggered(Key::Space) && (grounded || !_double_jump)) {
      if (!_double_jump && !grounded)
        _double_jump = true;
      if (grounded)
        player_ento.Transform().SetPosition(player_ento.Transform().GetPosition() + v3 { 0.0f, 0.2f, 0.0f });
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
        _target_distance += 0.1f;
      if (io.ButtonDown(pad, PadButton::RightBumper))
        _target_distance -= 0.1f;

      constexpr f32 AXIS_DEAD_ZONE = 0.3f;

      if (grounded) {
        v2 left = { io.GetAxis(pad, JoyStick::LeftX), io.GetAxis(pad, JoyStick::LeftY) };

        if (left.x < AXIS_DEAD_ZONE && left.x > -AXIS_DEAD_ZONE)
          left.x = 0.0f;
        if (left.y < AXIS_DEAD_ZONE && left.y > -AXIS_DEAD_ZONE)
          left.y = 0.0f;

        v3 front_impulse = front * -left.y;
        v3 right_impulse = right * -left.x;
        rb.AddLinearImpulse(front_impulse * MOVEMENT_SPEED * (f32)delta);
        rb.AddLinearImpulse(right_impulse * MOVEMENT_SPEED * (f32)delta);
      }
      if (io.ButtonTriggered(pad, PadButton::A) && (grounded || !_double_jump)) {
        if (!_double_jump && !grounded)
          _double_jump = true;
        if (grounded)
          player_ento.Transform().SetPosition(player_ento.Transform().GetPosition() + v3 { 0.0f, 0.1f, 0.0f });
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

      _score++;

      if (_score >= 4)
        _game_over = true;
      Scene::GetActiveScene()->RemoveEntity(Ento::FromComponent(other_rb));
    }

    // Platform

    Ento platform_ento = FromID(_platform_id);
    if (!platform_ento)
      return;

    v3 platform_dir = platform_pos[_platform_n] - platform_ento.Transform().GetPosition();
    platform_dir = normalize(platform_dir);

    v3 platform_movement = platform_dir * MOVEMENT_SPEED * 0.5f;
    platform_ento.Transform().SetPosition(platform_ento.Transform().GetPosition() + platform_movement * (f32)delta);

    f32 dist = length2(platform_pos[_platform_n] - platform_ento.Transform().GetPosition());
    if (epsilonEqual(dist, 0.0f, std::numeric_limits<f32>::epsilon()))
      _platform_n = (_platform_n + 1) % platform_pos.size();

    OBBCollider &platform_collider = platform_ento.GetComponent<OBBCollider>();
    if (platform_collider.SphereInside(sc))
      player_ento.Transform().SetPosition(player_ento.Transform().GetPosition() + platform_movement * (f32)delta);

    // Elevator

    Ento elevator_ento = FromID(_elevator_id);
    if (!elevator_ento)
      return;

    OBBCollider &elevator_collider = elevator_ento.GetComponent<OBBCollider>();
    Transform &elevator_transform = elevator_ento.Transform();
    v3 elevator_pos = elevator_transform.GetPosition();

    if (elevator_collider.SphereInside(sc)) {
      if (elevator_pos.y < elevator_height[1]) {
        elevator_pos.y += MOVEMENT_SPEED * (f32)delta * 0.2f;
        elevator_transform.SetPosition(elevator_pos);
      }
    } else {
      if (elevator_pos.y > elevator_height[0]) {
        elevator_pos.y -= MOVEMENT_SPEED * (f32)delta * 0.2f;
        elevator_transform.SetPosition(elevator_pos);
      }
    }
  }

  void PhysicsLevel::UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    if (!_game_over)
      MenuLevel::ShowBackMenu(window, frame_size, frame_pos, _show_menu);
    else
      ShowScoreMenu(window, frame_size, frame_pos);
  }

  void PhysicsLevel::Focused(Window &window, bool state) { }

} // namespace axl
