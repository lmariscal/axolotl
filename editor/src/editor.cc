#include <iostream>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>
#include <axolotl/renderer.h>
#include <imgui.h>

using namespace axl;

i32 main() {
  Axolotl::Init();
  Window window(1280, 720, "Axolotl Editor");
  Renderer *renderer = window.GetRenderer();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  while (window.Update()) {
    renderer->ClearScreen({ 0.13f, 0.13f, 0.13f });

    // log::debug("Delta: {}ms", window.GetDeltaTime());

    ImGui::ShowDemoWindow();
    window.Draw();
  }
}
