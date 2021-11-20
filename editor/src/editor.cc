#include <iostream>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>
#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <imgui.h>
#include <axolotl/terminal.h>

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
  terminal.set_min_log_level(ImTerm::message::severity::debug);

  // auto formatter = std::make_unique<spdlog::pattern_formatter>();
  // formatter->set_pattern(pattern);

  terminal.get_terminal_helper()->set_terminal_pattern("[%R:%S:%e] [user]: %v", ImTerm::message::type::user_input);
  terminal.get_terminal_helper()->set_terminal_pattern("[%R:%S:%e] [history]: %v", ImTerm::message::type::cmd_history_completion);
  terminal.get_terminal_helper()->set_terminal_pattern("[%R:%S:%e] [error]: %v", ImTerm::message::type::error);
  log::default_logger()->sinks().push_back(terminal.get_terminal_helper());
  spdlog::set_pattern("[%R:%S:%e] [%^%l%$]: %v");

  terminal.set_autocomplete_pos(ImTerm::position::nowhere);
  terminal.theme() = ImTerm::themes::cherry;

  TestScene scene;
  scene.Init();

  while (window.Update() && !terminal_data.quit_requested) {
    renderer->ClearScreen({ 0.13f, 0.13f, 0.13f });

    if (terminal_data.watch_shaders) {
      std::vector<Shader *> need_recompile = Axolotl::WatchShaders();
      for (Shader *shader : need_recompile)
        shader->Recompile();
    }

    ImGui::ShowDemoWindow();

    terminal.show();

    scene.Update(window.GetDeltaTime());
    scene.Draw();

    window.Draw();
  }
}
