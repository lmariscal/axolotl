#include "test_scene.h"

#include <axolotl/shader.h>
#include <axolotl/mesh.h>
#include <axolotl/axolotl.h>
#include <axolotl/camera.h>
#include <axolotl/window.h>
#include <imgui.h>

namespace axl {

  void TestScene::Init() {
    // Load serialized scene

    entt::entity camera = _registry.create();

    Camera &camera_component = _registry.emplace<Camera>(camera);
    camera_component.SetAsActive();

    entt::entity triangle = _registry.create();

    std::vector<f32> triangle_mesh = {
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f,  1.0f, 0.0f
    };
    Mesh &mesh = _registry.emplace<Mesh>(triangle, triangle_mesh);
    Transform &transform = _registry.emplace<Transform>(triangle);
    transform.SetPosition({ 0.0f, 0.0f, -5.0f });
    Shader &shader = _registry.emplace<Shader>(
        triangle,
        Axolotl::GetDistDir() + "res/shaders/testy.vert",
        Axolotl::GetDistDir() + "res/shaders/testy.frag"
      );
    shader.Compile();
  }

  void TestScene::Update(Window &window) {
    // Execute scene scripting, per entity or an overall script
    auto view = _registry.view<Camera>();
    for (auto entity : view) {
      Camera &camera = view.get<Camera>(entity);
      camera.Update(window);
    }
  }

} // namespace axl
