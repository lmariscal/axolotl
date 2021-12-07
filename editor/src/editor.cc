#include "dockspace.hh"
#include "test_scene.hh"
#include "ui.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/gui.hh>
#include <axolotl/renderer.hh>
#include <axolotl/shader.hh>
#include <axolotl/transform.hh>
#include <axolotl/window.hh>
#include <imgui.h>
#include <iostream>

using namespace axl;

void MainLoop(Window &window, TerminalData &terminal_data) {
  Renderer &renderer = window.GetRenderer();

  ImTerm::terminal<Terminal> terminal(terminal_data);
  terminal.get_terminal_helper()->Init();
  terminal.set_min_log_level(ImTerm::message::severity::debug);

  TestScene scene;
  Scene::SetActiveScene(&scene);

  FrameEditor frame_editor;

  IOManager &io = window.GetIOManager();
  DockSpace dock;

  bool last_fullscreen = false;

  constexpr f64 time_step = 1.0 / 100.0;
  f64 time_accumulator = 0.0;

  scene.Init(window);

  while (window.Update() && !terminal_data.quit_requested) {
    if (terminal_data.watch_shaders) { ShaderStore::ProcessQueue(); }

    bool show_frame = !(terminal_data.scene_playing && dock.data.fullscreen);
    if (frame_editor.focused && io.KeyTriggered(Key::Escape)) frame_editor.focused = false;
    if (io.KeyTriggered(Key::F11)) dock.data.fullscreen = !dock.data.fullscreen;

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

    if (terminal_data.scene_playing && !terminal_data.scene_paused) {
      time_accumulator += window.GetDeltaTime();
      while (time_accumulator >= time_step) {
        scene.Update(window, time_step);
        time_accumulator -= time_step;
      }
    }

    if (!show_frame) {
      v2i region_available = window.GetWindowFrameBufferSize();
      FrameBuffer::BindDefault();
      v3 color(33, 33, 33);
      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      renderer.Resize(region_available.x, region_available.y);
      window.SetFrameBufferSize(region_available);

      if (terminal_data.display_terminal) terminal.show();

      if (!frame_editor.focused && io.ButtonTriggered(MouseButton::Left) && !ImGui::GetIO().WantCaptureMouse) {
        frame_editor.focused = true;
      }
    } else {
      v2i region_available = frame_editor.GetRegionAvailable();
      if (region_available.x > 0 && region_available.y > 0) window.SetFrameBufferSize(region_available);

      if (dock.data.show_terminal) terminal.show();
      frame_editor.Bind(window);
      v3 color(33, 33, 33);
      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      renderer.Resize(frame_editor.GetRegionAvailable().x, frame_editor.GetRegionAvailable().y);
    }

    if (dock.data.show_world_editor) scene.Draw(renderer, dock.data.show_renderer && show_frame);

    if (show_frame) {
      frame_editor.Unbind(window);

      renderer.ClearScreen({ 0.13f, 0.13f, 0.13f });
      if (dock.data.show_world_editor) frame_editor.Draw(window, dock);
      if (dock.data.show_hierarchy) frame_editor.DrawEntityList(scene, dock);
      if (dock.data.show_inspector) frame_editor.DrawInspector(scene, dock);
    }

    scene.Focused(window, frame_editor.focused);

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
      if (io.KeyDown(Key::Q)) terminal_data.quit_requested = true;
    }
  }

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
