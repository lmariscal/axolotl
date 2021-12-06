#include "cs8502.hh"

#include <axolotl/ento.hh>
#include <axolotl/camera.hh>
#include <axolotl/iomanager.hh>
#include <axolotl/window.hh>
#include <axolotl/renderer.hh>
#include <axolotl/texture.hh>
#include <axolotl/axolotl.hh>
#include <axolotl/light.hh>

namespace axl {

  void CS8502::Init(Window &window) {
    Ento camera = CreateEntity();
    camera.Tag().value = "Camera";
    camera.AddComponent<Camera>(camera).SetAsActive(camera);

    Ento light = CreateEntity();
    light.Tag().value = "Light";
    light.AddComponent<Light>(LightType::Point, vec3(1.0f, 0.0f, 0.0f));

    window.GetRenderer().SetSkybox(new TextureCube(Axolotl::GetDistDir() + "res/textures/TropicalSunnyDay"));
  }

  void CS8502::Update(Window &window, f64 delta) {
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

  void CS8502::Focused(Window &window, bool state) {
    window.LockMouse(state);
  }

} // namespace axl
