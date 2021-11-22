#include <iostream>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>
#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <axolotl/gui.h>
#include <axolotl/transform.h>
#include <imgui.h>

#include "frame_editor.h"
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

  Transform transform;
  transform.SetPosition({ 0.0f, 0.0f, -5.0f });
  transform.SetRotation({ 0.0f, 0.0f, 0.0f, 0.0f });
  transform.SetScale({ 1.0f, 1.0f, 1.0f });
  nlohmann::json json;
  json["transform"] = transform.Serialize();
  log::debug("{}", json.dump(2));

  std::vector<u8> cbor = nlohmann::json::to_cbor(json);
  for (auto &byte : cbor)
    std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
  std::cout << '\n';

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
