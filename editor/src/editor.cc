#include <iostream>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>
#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <imgui.h>
#include <axolotl/terminal.h>

#include "frame_editor.h"
#include "test_scene.h"

using namespace axl;

i32 main() {
  Axolotl::Init();
  Window window(1920, 1080, "Axolotl Editor");
  Renderer *renderer = window.GetRenderer();

  ImGuiIO &io = ImGui::GetIO();
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.ConfigViewportsNoAutoMerge = true;

  TerminalData terminal_data;
  ImTerm::terminal<Terminal> terminal(terminal_data);
  terminal.get_terminal_helper()->Init();
  terminal.set_min_log_level(ImTerm::message::severity::debug);

  TestScene scene;
  scene.Init();

  FrameEditor frame_editor;
  frame_editor.SetBoundFrameRatio(false);

  IOManager *io_manager = window.GetIOManager();

  while (window.Update() && !terminal_data.quit_requested) {
    if (terminal_data.watch_shaders) {
      std::vector<Shader *> need_recompile = Axolotl::WatchShaders();
      for (Shader *shader : need_recompile)
        shader->Recompile();
    }

    scene.Update(window.GetDeltaTime());
    ImGui::ShowDemoWindow();
    terminal.show();

    frame_editor.Bind(window);
    scene.Draw();
    frame_editor.Unbind(window);
    frame_editor.Draw(window);

    renderer->ClearScreen({ 0.13f, 0.13f, 0.13f });
    window.Draw();

    if (io_manager->KeyDown(Key::LeftControl) || io_manager->KeyDown(Key::RightControl)) {
      if (io_manager->KeyDown(Key::Q))
        terminal_data.quit_requested = true;
    }
  }
}
