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
  Renderer *renderer = window.GetRenderer();

  ImTerm::terminal<Terminal> terminal(terminal_data);
  terminal.get_terminal_helper()->Init();
  terminal.set_min_log_level(ImTerm::message::severity::debug);

  TestScene scene;
  scene.Init();

  FrameEditor frame_editor;

  IOManager *io_manager = window.GetIOManager();
  DockSpace dock_space;

  while (window.Update() && !terminal_data.quit_requested) {
    if (terminal_data.watch_shaders) {
      std::vector<Shader *> need_recompile = Axolotl::WatchShaders();
      for (Shader *shader : need_recompile)
        shader->Recompile();
    }

    DockSpaceData dock_space_data;
    dock_space_data.terminal_data = &terminal_data;
    dock_space_data.resize_world_editor = &frame_editor.bound_frame_ratio;
    dock_space.Draw(window, dock_space_data);

    scene.Update(window);
    terminal.show();

    frame_editor.Bind(window);
    renderer->ClearScreen({ 149.0f / 255.0f, 117.0f / 255.0f, 205.0f / 255.0f });
    scene.Draw(*renderer);
    frame_editor.Unbind(window);
    frame_editor.Draw(window);

    frame_editor.DrawEntityList(scene);
    frame_editor.DrawInspector(scene);

    renderer->ClearScreen({ 0.13f, 0.13f, 0.13f });

    window.Draw();

    if (io_manager->KeyDown(Key::LeftControl) || io_manager->KeyDown(Key::RightControl)) {
      if (io_manager->KeyDown(Key::Q))
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

  io.ConfigViewportsNoAutoMerge = true;

  TerminalData terminal_data;

  MainLoop(window, terminal_data);
}
