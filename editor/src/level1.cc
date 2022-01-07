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

  void PhysicsLevel::Init(Window &window) {
    std::fstream file(Axolotl::GetDistDir() + "res/misc/project.json");
    AXL_ASSERT_MESSAGE(file.is_open(), "could not open scene file for PhysicsLevel");

    _world_data = json::parse(file);
    Deserialize(_world_data);
    file.close();

    _player_id = *uuid::from_string("c63d2009-5657-4304-a9b7-8110d9f8b4c2");
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
        if (obb.SphereInside(sc))
          grounded = true;
        return;
      }
    });

    if (grounded) {
      if (io.KeyDown(Key::W))
        rb.AddLinearImpulse(front);
      if (io.KeyDown(Key::S))
        rb.AddLinearImpulse(-front);
      if (io.KeyDown(Key::A))
        rb.AddLinearImpulse(right);
      if (io.KeyDown(Key::D))
        rb.AddLinearImpulse(-right);

      _double_jump = false;
    }

    if (io.KeyDown(Key::Left))
      _camera_position.x -= 0.1f;
    if (io.KeyDown(Key::Right))
      _camera_position.x += 0.1f;
    if (io.KeyDown(Key::Up))
      _camera_position.y += 0.1f;
    if (io.KeyDown(Key::Down))
      _camera_position.y -= 0.1f;

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
        rb.AddLinearImpulse(front_impulse);
        rb.AddLinearImpulse(right_impulse);
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

      constexpr f32 ROTATION_SPEED = 0.06f;

      if (right.x < AXIS_DEAD_ZONE && right.x > -AXIS_DEAD_ZONE)
        right.x = 0.0f;
      if (right.y < AXIS_DEAD_ZONE && right.y > -AXIS_DEAD_ZONE)
        right.y = 0.0f;

      _camera_position.x -= right.x * ROTATION_SPEED;
      _camera_position.y -= right.y * ROTATION_SPEED;
    }

    for (Ento &c : rb.colliding_with) {
      if (!c.HasComponent<RigidBody>())
        continue;

      RigidBody &other_rb = c.GetComponent<RigidBody>();

      if (!other_rb.is_trigger)
        continue;

      _score++;
      Scene::GetActiveScene()->RemoveEntity(Ento::FromComponent(other_rb));
    }
  }

  void PhysicsLevel::UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    IOManager &ioman = window.GetIOManager();
    if (ioman.KeyTriggered(Key::Escape))
      _show_menu = !_show_menu;

    if (!_show_menu)
      return;

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
    ImGui::Text("CSC8503 - Advanced Game Technologies");
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    if (ImGui::Button("Back to Main Menu")) {
      Scene::SetActiveScene(new MenuLevel());
      log::debug("AI Level");
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void PhysicsLevel::Focused(Window &window, bool state) { }

} // namespace axl
