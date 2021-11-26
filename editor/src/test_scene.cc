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
    entt::entity camera = CreateEntity();
    Ento &ento = GetComponent<Ento>(camera);
    ento.name = "Camera";

    Camera &camera_component = AddComponent<Camera>(camera);
    camera_component.SetAsActive();
    Transform &camera_transform = GetComponent<Transform>(camera);
    camera_transform.SetPosition({ -10.4f, 2.15f, -1.0f });
    camera_transform.SetRotation({ 7.5f, -4.4f, 0.0f });
    camera_component.UpdateVectors();


    ShaderPaths shader_paths = {
      Axolotl::GetDistDir() + "res/shaders/testy.vert",
      Axolotl::GetDistDir() + "res/shaders/testy.frag"
    };

    entt::entity sponza = CreateEntity();
    GetComponent<Ento>(sponza).name = "Sponza";
    AddComponent<Model>(sponza, "/home/coffee/dev/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf", shader_paths);

    entt::entity town = CreateEntity();
    GetComponent<Ento>(town).name = "Town";
    AddComponent<Model>(town, "/home/coffee/docs/models/town/scene.gltf", shader_paths);
    GetComponent<Transform>(town).SetPosition({ 0.0f, 0.0f, 20.0f });

    entt::entity backpack = CreateEntity();
    GetComponent<Ento>(backpack).name = "Backpack";
    AddComponent<Model>(backpack, "/home/coffee/docs/models/backpack-obj/backpack.obj", shader_paths);
    GetComponent<Transform>(backpack).SetPosition({ 0.0f, 20.0f, 0.0f });

    TextureStore::ProcessQueue();
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
  }

  void TestScene::Focused(Window &window, bool state) {
    window.LockMouse(state);
  }

} // namespace axl
