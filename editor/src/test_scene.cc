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
    Ento asd = CreateEntity();
  }

  void TestScene::Update(Window &window) {
    // Execute scene scripting, per entity or an overall script
    IOManager &io = window.GetIOManager();
    Camera *camera = Camera::GetActiveCamera();
    if (window.GetLockMouse()) {
      camera->RotateCamera(io.GetRelativePosition(), window);

      if (io.KeyDown(Key::W))
        camera->MoveCamera(CameraDirection::Front, window);
      if (io.KeyDown(Key::S))
        camera->MoveCamera(CameraDirection::Back, window);
      if (io.KeyDown(Key::A))
        camera->MoveCamera(CameraDirection::Left, window);
      if (io.KeyDown(Key::D))
        camera->MoveCamera(CameraDirection::Right, window);
      if (io.KeyDown(Key::Q))
        camera->MoveCamera(CameraDirection::Up, window);
      if (io.KeyDown(Key::E))
        camera->MoveCamera(CameraDirection::Down, window);
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
