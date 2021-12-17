#include "test_scene.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/material.hh>
#include <axolotl/model.hh>
#include <axolotl/renderer.hh>
#include <axolotl/texture.hh>
#include <axolotl/window.hh>
#include <imgui.h>

namespace axl {

  void TestScene::Init(Window &window) {
    Ento camera_ento = CreateEntity();
    camera_ento.Tag().value = "Camera";
    Camera &camera_component = camera_ento.AddComponent<Camera>();
    camera_component.SetAsActive();
    camera_ento.Transform().SetPosition(v3(-2.7f, 4.6f, -6.4f));
    camera_ento.Transform().SetRotationEuler(v3(67.5f, -29.5f, 0));
    camera_component.UpdateVectors(&camera_ento.Transform());

    Ento light_ento = CreateEntity();
    light_ento.Tag().value = "Light";
    Light &light_component = light_ento.AddComponent<Light>(LightType::Point, v3(1.0f), 0.6f);

    Ento cube_ento = CreateEntity();
    cube_ento.Tag().value = "Cube";
    std::vector<std::string> shader_paths = { Axolotl::GetDistDir() + "res/shaders/testy.vert",
                                              Axolotl::GetDistDir() + "res/shaders/testy.frag" };
    Model &cube_model = cube_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
    TextureStore::ProcessQueue();

    // window.GetRenderer().SetSkybox(new TextureCube(Axolotl::GetDistDir() + "res/textures/TropicalSunnyDay"));
  }

  void TestScene::Update(Window &window, f64 delta) {
    // Execute scene scripting, per entity or an overall script
    IOManager &io = window.GetIOManager();
    Camera *camera = Camera::GetActiveCamera();
    if (camera && window.GetLockMouse()) {
      Ento camera_ento = Camera::GetActiveCameraEnto();
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
  }

  void TestScene::Focused(Window &window, bool state) {
    window.LockMouse(state);
    focused = state;
  }

} // namespace axl
