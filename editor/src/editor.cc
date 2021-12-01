#include <iostream>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>
#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <axolotl/gui.h>
#include <axolotl/transform.h>
#include <imgui.h>

#include "ui.h"
#include "dockspace.h"
#include "test_scene.h"

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

  scene.Init();

  while (window.Update() && !terminal_data.quit_requested) {
    if (terminal_data.watch_shaders) {
      std::vector<Shader *> need_recompile = Axolotl::WatchShaders();
      for (Shader *shader : need_recompile)
        shader->Recompile();
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

    if (terminal_data.scene_playing && !terminal_data.scene_paused)
      scene.Update(window);

    if (!show_frame) {
      v2i region_available = window.GetWindowFrameBufferSize();
      v3 color(33, 33, 33);
      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      renderer.Resize(region_available.x, region_available.y);
      window.SetFrameBufferSize(region_available);

      if (terminal_data.display_terminal)
        terminal.show();

      if (!frame_editor.focused && io.ButtonTriggered(MouseButton::Left) && !ImGui::GetIO().WantCaptureMouse) {
        frame_editor.focused = true;
      }
    } else {
      v2i region_available = frame_editor.GetRegionAvailable();
      if (region_available.x > 0 && region_available.y > 0)
        window.SetFrameBufferSize(region_available);

      terminal.show();
      frame_editor.Bind(window);
      v3 color(33, 33, 33);
      renderer.ClearScreen(v4(color / 255.0f, 1.0f));
      renderer.Resize(frame_editor.GetRegionAvailable().x, frame_editor.GetRegionAvailable().y);
    }

    scene.Draw(renderer, show_frame);

    if (show_frame) {
      frame_editor.Unbind(window);

      renderer.ClearScreen({ 0.13f, 0.13f, 0.13f });
      frame_editor.Draw(window, dock);
      frame_editor.DrawEntityList(scene);
      frame_editor.DrawInspector(scene);
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
      if (io.KeyDown(Key::Q))
        terminal_data.quit_requested = true;
    }
  }
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
