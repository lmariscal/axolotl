#include "dockspace.hh"
#include "menu.hh"
#include "ui.hh"

#include <ImGuizmo.h>
#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/gui.hh>
#include <axolotl/line.hh>
#include <axolotl/physics.hh>
#include <axolotl/renderer.hh>
#include <axolotl/shader.hh>
#include <axolotl/transform.hh>
#include <axolotl/window.hh>
#include <fstream>
#include <imgui.h>
#include <iostream>

using namespace axl;

v3 target_position;
v2 eye_position;
f32 target_distance = 5.0f;
v2 start_dragging_pos;
v2 start_rotating_pos;

i32 frame_count = 0;
f64 physics_time = 0.0;
f64 physics_time_total = 0.0;
f64 physics_time_debug = 0.0;
f64 imgui_time = 0.0;
f64 update_time = 0.0;
f64 physics_time_update_sum = 0.0;
f64 physics_time_total_sum = 0.0;
f64 imgui_time_sum = 0.0;
f64 update_time_sum = 0.0;
f64 last_time_sum = 0.0;

i32 physics_update_count_sum = 0;
f32 physics_update_count = 0;

void UpdateEditorCamera(Window &window,
                        Camera &camera,
                        Transform &transform,
                        DockSpaceData &dock_space_data,
                        f32 step) {

  v3 movement { sin(eye_position.x), eye_position.y, cos(eye_position.x) };
  v3 pos = normalize(movement) * target_distance;
  pos += target_position;
  v3 right = normalize(cross({ 0.0f, 1.0f, 0.0f }, (pos - target_position)));

  m4 view = lookAt(pos, target_position, { 0.0f, 1.0f, 0.0f });

  camera.SetCustomViewMatrix(view);

  if (!dock_space_data.hover_frame_editor)
    return;

  IOManager &io = window.GetIOManager();
  if (io.ButtonTriggered(MouseButton::Middle)) {
    start_dragging_pos = io.GetAbsolutePosition();
  }

  if (io.ButtonDown(MouseButton::Middle)) {

    v2 delta = io.GetAbsolutePosition() - start_dragging_pos;
    delta *= step;

    if (io.KeyDown(Key::LeftShift) || io.KeyDown(Key::RightShift)) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

      v3 right_delta = right * delta.x * 2.0f;
      v3 front_delta = v3(0.0f, 1.0f, 0.0f) * delta.y * -2.0f;
      target_position -= right_delta;
      target_position -= front_delta;
    } else {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

      eye_position.y += delta.y * 2.0f;
      eye_position.x -= delta.x * 0.3f;
    }

    start_dragging_pos = io.GetAbsolutePosition();
  }

  if (io.ButtonReleased(MouseButton::Middle)) {
    start_dragging_pos = v2 {};
  }

  if (io.WheelMoved()) {
    target_distance -= io.GetWheelMovement().y * step * 100.0f;
    target_distance = max(target_distance, 2.0f);
  }
}

void MainLoop(Window &window, TerminalData &terminal_data) {
  Renderer &renderer = window.GetRenderer();

  ImTerm::terminal<Terminal> terminal(terminal_data);
  terminal.get_terminal_helper()->Init();
  terminal.set_min_log_level(ImTerm::message::severity::debug);

  Camera editor_camera;
  editor_camera.SetFov(90.0f);
  Transform editor_camera_transform;
  editor_camera_transform.SetRotationEuler({ 67.0f, -29.5f, 0.0f });

  DockSpace dock;

  Scene::SetActiveScene(new MenuLevel());
  Scene::new_scene = false;

  FrameEditor frame_editor;

  IOManager &io = window.GetIOManager();

  bool last_fullscreen = false;

  constexpr f64 time_step = 1.0 / 60.0;
  f64 time_accumulator = 0.0;

  f64 update_time_start;
  f64 update_time_end;
  f64 imgui_time_start;
  f64 imgui_time_end;
  f64 physics_time_update_start;
  f64 phyisics_time_update_end;
  f64 physics_time_total_start;
  f64 phyisics_time_total_end;

  Scene::GetActiveScene()->Init(window);

  last_time_sum = window.GetTime();

  while (window.Update() && !terminal_data.quit_requested) {
    dock.data.scene = Scene::GetActiveScene();
    Scene *scene = dock.data.scene;

    UpdateEditorCamera(window, editor_camera, editor_camera_transform, dock.data, window.GetDeltaTime());
    if (terminal_data.watch_shaders) {
      ShaderStore::ProcessQueue();
    }

    bool show_frame = !(terminal_data.scene_playing && dock.data.fullscreen);
    if (frame_editor.focused && io.KeyTriggered(Key::Escape))
      frame_editor.focused = false;
    if (io.KeyTriggered(Key::F11))
      dock.data.fullscreen = !dock.data.fullscreen;

    if (!show_frame != last_fullscreen) {
      if (show_frame) {
        dock.first_iteration = true;
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
      } else {
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
      }
    }
    last_fullscreen = !show_frame;

    dock.data.terminal = &terminal_data;
    if (show_frame)
      dock.Draw(window);

    update_time_start = window.GetTime();
    if (terminal_data.scene_playing && !terminal_data.scene_paused) {
      time_accumulator += window.GetDeltaTime();
      physics_time_total_start = window.GetTime();
      while (time_accumulator >= time_step) {
        physics_time_update_start = window.GetTime();

        physics_update_count_sum++;
        scene->PhysicsUpdate(time_step);
        time_accumulator -= time_step;

        phyisics_time_update_end = window.GetTime();
        physics_time_update_sum += phyisics_time_update_end - physics_time_update_start;
      }
      phyisics_time_total_end = window.GetTime();

      scene->Update(window, time_step);

      if (!dock.data.fullscreen)
        scene->UpdateGUI(window, terminal_data.frame_size, terminal_data.frame_pos);
      else
        scene->UpdateGUI(window, window.GetWindowFrameBufferSize(), v2(0));
    }
    update_time_end = window.GetTime();
    if (Scene::new_scene) {
      delete scene;
      Scene::new_scene = false;
      window.Draw();
      log::debug("changing scene...");
      Scene::GetActiveScene()->Init(window);
      continue;
    }

    scene->Focused(window, frame_editor.focused);

    // Actual Scene Rendering START

    if (!show_frame) {

      v2i region_available = window.GetWindowFrameBufferSize();
      FrameBuffer::BindDefault();
      v3 color(33, 33, 33);
      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      renderer.Resize(region_available.x, region_available.y);
      window.SetFrameBufferSize(region_available);

      if (terminal_data.display_terminal)
        terminal.show();

      if (dock.data.terminal->scene_playing && !dock.data.terminal->scene_paused) {
        scene->Draw(renderer, dock.data.show_renderer && show_frame);
      } else {
        scene->Draw(renderer, dock.data.show_renderer && show_frame, &editor_camera, &editor_camera_transform);
      }

    } else {

      v2i region_available = frame_editor.GetRegionAvailable();
      if (region_available.x > 0 && region_available.y > 0)
        window.SetFrameBufferSize(region_available);

      frame_editor.Bind(window);
      v3 color(33, 33, 33);
      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      renderer.Resize(frame_editor.GetRegionAvailable().x, frame_editor.GetRegionAvailable().y);
      renderer.SetMeshWireframe(dock.data.show_wireframe);

      if (dock.data.show_world_editor) {
        if (dock.data.terminal->scene_playing && !dock.data.terminal->scene_paused) {
          scene->Draw(renderer, dock.data.show_renderer && show_frame);
        } else {
          scene->Draw(renderer, dock.data.show_renderer && show_frame, &editor_camera, &editor_camera_transform);
        }
      }

      frame_editor.Unbind(window);

      imgui_time_start = window.GetTime();

      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      if (dock.data.show_terminal)
        terminal.show();
      if (dock.data.show_world_editor)
        frame_editor.Draw(window, dock, &editor_camera);
      if (dock.data.show_hierarchy)
        frame_editor.DrawEntityList(*scene, dock);
      if (dock.data.show_inspector)
        frame_editor.DrawInspector(*scene, dock);

      imgui_time_end = window.GetTime();
    }

    // Actual Scene Rendering END

    f64 now = window.GetTime();
    if (now - last_time_sum >= 0.25) {
      last_time_sum = now;

      update_time = update_time_sum / (f32)frame_count;
      update_time_sum = 0.0;

      imgui_time = imgui_time_sum / (f32)frame_count;
      imgui_time_sum = 0.0;

      physics_time_total = physics_time_total_sum / (f32)frame_count;
      physics_time_total_sum = 0.0;

      physics_time = physics_time_update_sum / (f32)physics_update_count_sum;
      physics_time_update_sum = 0.0;

      physics_time_debug = Physics::total_physics_time / (f32)physics_update_count_sum;
      Physics::total_physics_time = 0.0;

      physics_update_count = physics_update_count_sum / (f32)frame_count;
      physics_update_count_sum = 0;

      frame_count = 0;
    }

    update_time_sum += update_time_end - update_time_start;
    imgui_time_sum += imgui_time_end - imgui_time_start;
    physics_time_total_sum += phyisics_time_total_end - physics_time_total_start;

    if (dock.data.show_performance) {
      ImGui::Begin("Performance", &dock.data.show_performance);
      const RendererPerformance &performance = renderer.GetPerformance();
      f64 imgui_time = imgui_time_end - imgui_time_start;
      f64 update_time = update_time_end - update_time_start;

      ImGui::Text("FPS: %u", performance.fps);
      ImGui::Text("Delta: %.2fms", performance.delta_time * 1000.0);
      ImGui::Text("Meshes: %u", performance.mesh_count);
      ImGui::Text("Physics Total: %.2fms", physics_time_total * 1000.0);
      ImGui::Text("Physics Update: %.2fms", physics_time * 1000.0);
      ImGui::Text("Physics Debug: %.2fms", physics_time_debug * 1000.0);
      ImGui::Text("Physics Update Count: %.2f per frame", physics_update_count);
      ImGui::Text("Vertices: %u", performance.vertex_count);
      ImGui::Text("Triangles: %u", performance.triangle_count);
      ImGui::Text("Draw Calls: %u", performance.draw_calls);
      ImGui::Text("ImGui Time: %.2fms", imgui_time * 1000.0);
      ImGui::Text("Update Time: %.2fms", update_time * 1000.0);
      ImGui::Text("Main Draw Time: %.2fms", performance.main_draw_time * 1000.0);
      ImGui::Text("Post Draw Time: %.2fms", performance.post_draw_time * 1000.0);
      ImGui::Text("GPU Render Time: %.2fms", performance.gpu_render_time);
      ImGui::Text("CPU Render Time: %.2fms", performance.cpu_render_time * 1000.0);
      ImGui::Text("Organization Time: %.2fms", performance.organization_time * 1000.0);
      ImGui::Text("Light Update Time: %.2fms", performance.lights_time * 1000.0);
      ImGui::End();
    }

    window.Draw();
    frame_count++;

    if (dock.data.fullscreen && terminal_data.scene_playing && io.KeyTriggered(Key::GraveAccent)) {
      // terminal_data.scene_paused = !terminal_data.scene_paused;
      terminal_data.display_terminal = !terminal_data.display_terminal;
      if (terminal_data.display_terminal) {
        v2 window_size = window.GetFrameBufferSize();
        ImGui::SetWindowSize("Terminal", v2(window_size.x * 0.4f, window_size.y * 0.4f));
        ImGui::SetWindowPos("Terminal", v2(20.0f));
      }
    }
    if (io.KeyDown(Key::LeftControl) || io.KeyDown(Key::RightControl)) {
      if (io.KeyTriggered(Key::O)) {
        dock.LoadProject();
      }
      if (io.KeyTriggered(Key::S)) {
        if (dock.data.project_path.empty())
          dock.SelectProjectPath();
        dock.SaveProject();
      }
      if (io.KeyDown(Key::LeftShift) || io.KeyDown(Key::RightShift)) {
        if (io.KeyTriggered(Key::S)) {
          dock.SaveProject();
        }
        if (io.KeyDown(Key::Q)) {
          terminal_data.quit_requested = true;
        }
      }
    }
  }

  delete Scene::GetActiveScene();
  terminal.get_terminal_helper()->Terminate();
}

i32 main() {
  Axolotl::Init();
  Window window(1920, 1080, "Axolotl Editor");

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // io.ConfigViewportsNoAutoMerge = true;

  TerminalData terminal_data;

  MainLoop(window, terminal_data);
}
