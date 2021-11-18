#include <iostream>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>
#include <axolotl/renderer.h>
#include <axolotl/mesh.h>
#include <axolotl/shader.h>
#include <imgui.h>

using namespace axl;

i32 main() {
  Axolotl::Init();
  Window window(1280, 720, "Axolotl Editor");
  Renderer *renderer = window.GetRenderer();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  Mesh mesh({
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
  });

  Shader shader;
  shader.Load(ShaderType::Vertex, Axolotl::GetDistDir() + "res/shaders/testy.vert");
  shader.Load(ShaderType::Fragment, Axolotl::GetDistDir() + "res/shaders/testy.frag");
  shader.Compile();

  while (window.Update()) {
    renderer->ClearScreen({ 0.13f, 0.13f, 0.13f });

    if (ImGui::Button("Reload")) {
      if (shader.Reload(ShaderType::Vertex))
        shader.Recompile();
    }
    // log::debug("Delta: {}ms", window.GetDeltaTime());

    ImGui::ShowDemoWindow();
    window.Draw();
  }
}
