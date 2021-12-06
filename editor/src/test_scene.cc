#include "test_scene.hh"

#include <axolotl/material.hh>
#include <axolotl/model.hh>
#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/window.hh>
#include <axolotl/texture.hh>
#include <axolotl/ento.hh>
#include <imgui.h>

namespace axl {

  void TestScene::Init(Window &window) {
    Ento camera = CreateEntity();
    camera.Tag().value = "Camera";
    camera.AddComponent<Camera>(camera).SetAsActive(camera);
  }

  void TestScene::Update(Window &window, f64 delta) {
    // Execute scene scripting, per entity or an overall script
    IOManager &io = window.GetIOManager();
    Camera *camera = Camera::GetActiveCamera();
    if (camera && window.GetLockMouse()) {
      Ento camera_ento = Camera::GetActiveCameraEnto();
      camera->RotateCamera(camera_ento, io.GetRelativePosition(), delta);

      if (io.KeyDown(Key::W))
        camera->MoveCamera(camera_ento, CameraDirection::Front, delta);
      if (io.KeyDown(Key::S))
        camera->MoveCamera(camera_ento, CameraDirection::Back, delta);
      if (io.KeyDown(Key::A))
        camera->MoveCamera(camera_ento, CameraDirection::Left, delta);
      if (io.KeyDown(Key::D))
        camera->MoveCamera(camera_ento, CameraDirection::Right, delta);
      if (io.KeyDown(Key::Q))
        camera->MoveCamera(camera_ento, CameraDirection::Up, delta);
      if (io.KeyDown(Key::E))
        camera->MoveCamera(camera_ento, CameraDirection::Down, delta);
    }

    // ImGui::Begin("Test Scene");
    // if (ImGui::Button("Serialize"))
    //   log::info("Scene:\n{}", Serialize().dump(2));
    // ImGui::End();
  }

  void TestScene::Focused(Window &window, bool state) {
    window.LockMouse(state);
  }

} // namespace axl
