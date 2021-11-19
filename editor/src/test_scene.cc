#include "test_scene.h"

#include <axolotl/shader.h>
#include <axolotl/mesh.h>
#include <axolotl/axolotl.h>
#include <imgui.h>

namespace axl {

  void TestScene::Init() {
    // Load serialized scene

    entt::entity triangle = _registry.create();

    std::vector<f32> triangle_mesh = {
      -0.5f, -0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.0f,  0.5f, 0.0f
    };
    Mesh &mesh = _registry.emplace<Mesh>(triangle, triangle_mesh);

    Shader &shader = _registry.emplace<Shader>(triangle);
    shader.Load(ShaderType::Vertex, Axolotl::GetDistDir() + "res/shaders/testy.vert");
    shader.Load(ShaderType::Fragment, Axolotl::GetDistDir() + "res/shaders/testy.frag");
    shader.Compile();
  }

  void TestScene::Update(f32 delta) {
    // Execute scene scripting, per entity or an overall script
    ImGui::ShowDemoWindow();
  }

} // namespace axl
