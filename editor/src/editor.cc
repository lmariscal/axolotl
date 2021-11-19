#include <iostream>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>
#include <axolotl/shader.h>
#include <axolotl/renderer.h>
#include <imgui.h>

#include "test_scene.h"

using namespace axl;

i32 main() {
  Axolotl::Init();
  Window window(1280, 720, "Axolotl Editor");
  Renderer *renderer = window.GetRenderer();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.ConfigViewportsNoAutoMerge = true;

  TestScene scene;
  scene.Init();

  while (window.Update()) {
    renderer->ClearScreen({ 0.13f, 0.13f, 0.13f });
    std::vector<Shader *> need_recompile = Axolotl::WatchShaders();
    for (Shader *shader : need_recompile)
      shader->Recompile();

    scene.Update(window.GetDeltaTime());
    scene.Draw();

    window.Draw();
  }
}
