#include "test_scene.h"

#include <axolotl/material.h>
#include <axolotl/model.h>
#include <axolotl/axolotl.h>
#include <axolotl/camera.h>
#include <axolotl/window.h>
#include <axolotl/texture.h>
#include <imgui.h>

namespace axl {

  void TestScene::Init() {
    // Load serialized scene

    entt::entity camera = CreateEntity();
    Ento &ento = GetComponent<Ento>(camera);
    ento.name = "Camera";

    Camera &camera_component = AddComponent<Camera>(camera);
    camera_component.SetAsActive();
    Transform &camera_transform = GetComponent<Transform>(camera);
    camera_transform.SetPosition({ 0.0f, 0.0f, -5.0f });

    _triangle = CreateEntity();
    Ento &triangle_ento = GetComponent<Ento>(_triangle);
    triangle_ento.name = "Triangle";

    ShaderPaths shader_paths = {
      Axolotl::GetDistDir() + "res/shaders/testy.vert",
      Axolotl::GetDistDir() + "res/shaders/testy.frag"
    };
    Model &model = AddComponent<Model>(_triangle, "/home/coffee/docs/models/backpack-obj/backpack.obj", shader_paths);
    // Mesh &mesh = AddComponent<Mesh>(_triangle, Mesh::CreateCube());

    Transform &transform = AddComponent<Transform>(_triangle, v3(0.0f));
    Texture &texture = AddComponent<Texture>(_triangle, Axolotl::GetDistDir() + "res/textures/crate0_diffuse.png");
    texture.Init();
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

  void TestScene::Focused(Window &window, bool state) {
    window.LockMouse(state);
  }

} // namespace axl
