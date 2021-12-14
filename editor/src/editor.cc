#include "dockspace.hh"
#include "test_scene.hh"
#include "ui.hh"

#include <ImGuizmo.h>
#include <axolotl/axolotl.hh>
#include <axolotl/gui.hh>
#include <axolotl/renderer.hh>
#include <axolotl/shader.hh>
#include <axolotl/transform.hh>
#include <axolotl/window.hh>
#include <fstream>
#include <imgui.h>
#include <iostream>

using namespace axl;

void MainLoop(Window &window, TerminalData &terminal_data) {
  Renderer &renderer = window.GetRenderer();

  ImTerm::terminal<Terminal> terminal(terminal_data);
  terminal.get_terminal_helper()->Init();
  terminal.set_min_log_level(ImTerm::message::severity::debug);

  DockSpace dock;

  dock.data.scene = new TestScene();
  Scene::SetActiveScene(dock.data.scene);

  FrameEditor frame_editor;

  IOManager &io = window.GetIOManager();

  bool last_fullscreen = false;

  constexpr f64 time_step = 1.0 / 100.0;
  f64 time_accumulator = 0.0;

  f64 update_time_start;
  f64 update_time_end;
  f64 imgui_starttime;
  f64 imgui_endtime;

  Scene &scene = *dock.data.scene;
  scene.Init(window);

  while (window.Update() && !terminal_data.quit_requested) {
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

    if (show_frame) {
      dock.data.terminal = &terminal_data;
      dock.Draw(window);
    }

    update_time_start = window.GetTime();
    if (terminal_data.scene_playing && !terminal_data.scene_paused) {
      time_accumulator += window.GetDeltaTime();
      while (time_accumulator >= time_step) {
        scene.Update(window, time_step);
        time_accumulator -= time_step;
      }
    }
    update_time_end = window.GetTime();

    scene.Focused(window, frame_editor.focused);

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

      scene.Draw(renderer, dock.data.show_renderer && show_frame);

    } else {

      v2i region_available = frame_editor.GetRegionAvailable();
      if (region_available.x > 0 && region_available.y > 0)
        window.SetFrameBufferSize(region_available);

      frame_editor.Bind(window);
      v3 color(33, 33, 33);
      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      renderer.Resize(frame_editor.GetRegionAvailable().x, frame_editor.GetRegionAvailable().y);
      renderer.SetMeshWireframe(dock.data.show_wireframe);

      if (dock.data.show_world_editor)
        scene.Draw(renderer, dock.data.show_renderer && show_frame);

      frame_editor.Unbind(window);

      imgui_starttime = window.GetTime();

      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      if (dock.data.show_terminal)
        terminal.show();
      if (dock.data.show_world_editor)
        frame_editor.Draw(window, dock);
      if (dock.data.show_hierarchy)
        frame_editor.DrawEntityList(scene, dock);
      if (dock.data.show_inspector)
        frame_editor.DrawInspector(scene, dock);

      imgui_endtime = window.GetTime();
    }

    // Actual Scene Rendering END

    if (dock.data.show_performance) {
      ImGui::Begin("Performance", &dock.data.show_performance);
      const RendererPerformance &performance = renderer.GetPerformance();
      f64 imgui_time = imgui_endtime - imgui_starttime;
      f64 update_time = update_time_end - update_time_start;

      ImGui::Text("FPS: %u", performance.fps);
      ImGui::Text("Delta: %.2fms", performance.delta_time * 1000.0);
      ImGui::Text("Meshes: %u", performance.mesh_count);
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

  delete dock.data.scene;
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
