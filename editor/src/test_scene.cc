#include "test_scene.h"

#include <axolotl/material.h>
#include <axolotl/model.h>
#include <axolotl/axolotl.h>
#include <axolotl/camera.h>
#include <axolotl/window.h>
#include <axolotl/texture.h>
#include <axolotl/ento.h>
#include <imgui.h>

namespace axl {

  void TestScene::Init() {
    Ento camera = CreateEntity();
    camera.Tag().value = "camera";
    camera.AddComponent<Camera>().SetAsActive(camera);
  }

  void TestScene::Update(Window &window) {
    // Execute scene scripting, per entity or an overall script
    IOManager &io = window.GetIOManager();
    f32 delta = window.GetDeltaTime();
    Camera *camera = Camera::GetActiveCamera();
    if (camera && window.GetLockMouse()) {
      camera->RotateCamera(io.GetRelativePosition(), delta);

      if (io.KeyDown(Key::W))
        camera->MoveCamera(CameraDirection::Front, delta);
      if (io.KeyDown(Key::S))
        camera->MoveCamera(CameraDirection::Back, delta);
      if (io.KeyDown(Key::A))
        camera->MoveCamera(CameraDirection::Left, delta);
      if (io.KeyDown(Key::D))
        camera->MoveCamera(CameraDirection::Right, delta);
      if (io.KeyDown(Key::Q))
        camera->MoveCamera(CameraDirection::Up, delta);
      if (io.KeyDown(Key::E))
        camera->MoveCamera(CameraDirection::Down, delta);
    }

    ImGui::Begin("Test Scene");
    if (ImGui::Button("Serialize"))
      log::info("Scene:\n{}", Serialize().dump(2));
    ImGui::End();
  }

  void TestScene::Focused(Window &window, bool state) {
    window.LockMouse(state);
  }

} // namespace axl
