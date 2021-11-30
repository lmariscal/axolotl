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

  Scene::SetActiveScene(new TestScene());
  auto scene = Scene::GetActiveScene();

  FrameEditor frame_editor;

  IOManager &io = window.GetIOManager();
  DockSpace dock_space;

  while (window.Update() && !terminal_data.quit_requested) {
    if (terminal_data.watch_shaders) {
      std::vector<Shader *> need_recompile = Axolotl::WatchShaders();
      for (Shader *shader : need_recompile)
        shader->Recompile();
    }

    bool no_frame = terminal_data.scene_playing && frame_editor.frame_focused && frame_editor.fullscreen_play;
    if (frame_editor.frame_focused && io.KeyTriggered(Key::Escape))
      frame_editor.frame_focused = false;

    if (!no_frame) {
      DockSpaceData dock_space_data;
      dock_space_data.terminal_data = &terminal_data;
      dock_space_data.resize_world_editor = &frame_editor.bound_frame_ratio;
      dock_space.Draw(window, dock_space_data);
    }

    if (terminal_data.scene_playing && !terminal_data.scene_paused)
      scene->Update(window);

    if (no_frame) {
      v2i region_available = window.GetWindowFrameBufferSize();
      renderer.ClearScreen({ 149.0f / 255.0f, 117.0f / 255.0f, 205.0f / 255.0f });
      renderer.Resize(region_available.x, region_available.y);
      window.SetFrameBufferSize(region_available);
    } else {
      v2i region_available = frame_editor.GetRegionAvailable();
      if (region_available.x > 0 && region_available.y > 0)
        window.SetFrameBufferSize(region_available);

      terminal.show();
      frame_editor.Bind(window);
      renderer.ClearScreen({ 149.0f / 255.0f, 117.0f / 255.0f, 205.0f / 255.0f });
      renderer.Resize(frame_editor.GetRegionAvailable().x, frame_editor.GetRegionAvailable().y);
    }

    scene->Draw(renderer);

    if (!no_frame) {
      frame_editor.Unbind(window);
      renderer.ClearScreen({ 0.13f, 0.13f, 0.13f });
      frame_editor.Draw(window, terminal_data);
      frame_editor.DrawEntityList(*scene);
      frame_editor.DrawInspector(*scene);
    }

    scene->Focused(window, frame_editor.frame_focused);

    window.Draw();

    if (terminal_data.scene_playing && io.KeyTriggered(Key::GraveAccent))
      terminal_data.scene_paused = !terminal_data.scene_paused;
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
