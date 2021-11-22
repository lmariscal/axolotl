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

    entt::entity camera = CreateEntity();
    _registry.get<Ento>(camera).name = "Camera";

    Camera &camera_component = AddComponent<Camera>(camera);
    camera_component.SetAsActive();

    _triangle = CreateEntity();
    _registry.get<Ento>(_triangle).name = "Triangle";

    std::vector<f32> triangle_mesh = {
      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       0.0f,  1.0f, 0.0f
    };
    Mesh &mesh = AddComponent<Mesh>(_triangle, triangle_mesh);
    Transform &transform = AddComponent<Transform>(_triangle, v3(0.0f, 0.0f, -5.0f));
    auto j = transform.Serialize();
    log::debug("j {}", j.dump(2));
    Shader &shader = AddComponent<Shader>(
        _triangle,
        Axolotl::GetDistDir() + "res/shaders/testy.vert",
        Axolotl::GetDistDir() + "res/shaders/testy.frag"
      );
    shader.Compile();
  }

  void TestScene::Update(Window &window) {
    // Execute scene scripting, per entity or an overall script
    IOManager *io_manager = window.GetIOManager();
    auto view = _registry.view<Camera>();
    for (auto entity : view) {
      Camera &camera = view.get<Camera>(entity);

      if (window.GetLockMouse()) {
        camera.RotateCamera(io_manager->GetRelativePosition(), window);

        if (io_manager->KeyDown(Key::W))
          camera.MoveCamera(CameraDirection::Front, window);
        if (io_manager->KeyDown(Key::S))
          camera.MoveCamera(CameraDirection::Back, window);
        if (io_manager->KeyDown(Key::A))
          camera.MoveCamera(CameraDirection::Left, window);
        if (io_manager->KeyDown(Key::D))
          camera.MoveCamera(CameraDirection::Right, window);
        if (io_manager->KeyDown(Key::Q))
          camera.MoveCamera(CameraDirection::Up, window);
        if (io_manager->KeyDown(Key::E))
          camera.MoveCamera(CameraDirection::Down, window);
      }
    }

    if (io_manager->KeyTriggered(Key::L)) {
      window.LockMouse(!window.GetLockMouse());
    }

    if (io_manager->KeyDown(Key::R)) {
      Transform &t = GetComponent<Transform>(_triangle);
      t.SetRotation(t.GetRotation() + v3(0.0f, 0.0f, 30.0f * window.GetDeltaTime()));
    }
  }

} // namespace axl
