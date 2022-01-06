#include "test_scene.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/geometry.hh>
#include <axolotl/line.hh>
#include <axolotl/material.hh>
#include <axolotl/model.hh>
#include <axolotl/physics.hh>
#include <axolotl/renderer.hh>
#include <axolotl/texture.hh>
#include <axolotl/window.hh>
#include <imgui.h>

namespace axl {

  uuid cube_id;
  uuid cube_other_id;
  uuid sphere_id;

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

    // Ento container = CreateEntity();
    // container.Tag().value = "Container";

    Ento cube_ento = CreateEntity();
    cube_id = cube_ento.id;
    cube_ento.Tag().value = "Cube";
    std::vector<std::string> shader_paths = { Axolotl::GetDistDir() + "res/shaders/testy.vert",
                                              Axolotl::GetDistDir() + "res/shaders/testy.frag" };
    cube_ento.AddComponent<OBBCollider>(cube_ento.Transform().GetPosition(), v3(1.0f));
    cube_ento.AddComponent<RigidBody>(1.0);
    Model &cube_model = cube_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
    TextureStore::ProcessQueue();

    // container.AddChild(cube_ento);

    Ento cube_other_ento = CreateEntity();
    cube_other_id = cube_other_ento.id;
    cube_other_ento.Tag().value = "CubeOther";
    cube_other_ento.Transform().SetPosition(v3(3.0f, 0.0f, 0.0f));
    cube_other_ento.AddComponent<OBBCollider>(cube_other_ento.Transform().GetPosition(), v3(1.0f));
    cube_other_ento.AddComponent<RigidBody>(1.0);
    Model &cube_other_model =
      cube_other_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
    TextureStore::ProcessQueue();

    Ento cube_other2_ento = CreateEntity();
    cube_other2_ento.Tag().value = "CubeOther2";
    cube_other2_ento.Transform().SetPosition(v3(3.0f, 0.0f, 0.0f));
    cube_other2_ento.AddComponent<OBBCollider>(cube_other2_ento.Transform().GetPosition(), v3(1.0f));
    cube_other2_ento.AddComponent<RigidBody>(0.0);
    Model &cube_other2_model =
      cube_other2_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
    TextureStore::ProcessQueue();

    Ento sphere_ento = CreateEntity();
    sphere_id = sphere_ento.id;
    sphere_ento.Tag().value = "Sphere";
    sphere_ento.Transform().SetPosition(v3(-3.0f, 0.0f, 0.0f));
    sphere_ento.Transform().SetScale(v3(0.5f));
    Model &sphere_model = sphere_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Sphere.fbx", shader_paths);
    sphere_ento.AddComponent<SphereCollider>(sphere_ento.Transform().GetPosition(),
                                             sphere_ento.Transform().GetScale().x);
    sphere_ento.AddComponent<RigidBody>(1.0);

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
        camera->MoveCamera(CameraDirection::Down, delta);
      if (io.KeyDown(Key::E))
        camera->MoveCamera(CameraDirection::Up, delta);

      Pad pad = Pad::Last;
      for (i32 p = 0; p < (i32)Pad::Last; ++p) {
        if (io.PadPresent((Pad)p)) {
          pad = (Pad)p;
          break;
        }
      }

      if (pad != Pad::Last) {
        if (io.ButtonDown(pad, PadButton::LeftBumper))
          camera->MoveCamera(CameraDirection::Down, delta);
        if (io.ButtonDown(pad, PadButton::RightBumper))
          camera->MoveCamera(CameraDirection::Up, delta);

        constexpr f32 AXIS_DEAD_ZONE = 0.3f;

        f32 left_x = io.GetAxis(pad, JoyStick::LeftX);
        f32 left_y = io.GetAxis(pad, JoyStick::LeftY);

        if (left_x > AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Right, delta * left_x);
        if (left_x < -AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Left, delta * -left_x);

        if (left_y > AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Back, delta * left_y);
        if (left_y < -AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Front, delta * -left_y);

        // --------------------------------------------------------------------------------

        f32 right_x = io.GetAxis(pad, JoyStick::RightX);
        f32 right_y = io.GetAxis(pad, JoyStick::RightY);

        constexpr f32 ROTATION_SPEED = 6.0f;

        if (right_x > AXIS_DEAD_ZONE)
          camera->RotateCamera(v2(right_x, 0.0f), delta * ROTATION_SPEED);
        if (right_x < -AXIS_DEAD_ZONE)
          camera->RotateCamera(v2(right_x, 0.0f), delta * ROTATION_SPEED);

        if (right_y > AXIS_DEAD_ZONE)
          camera->RotateCamera(v2(0.0f, right_y), delta * ROTATION_SPEED);
        if (right_y < -AXIS_DEAD_ZONE)
          camera->RotateCamera(v2(0.0f, right_y), delta * ROTATION_SPEED);
      }
    }
  }

  void TestScene::Focused(Window &window, bool state) {
    window.LockMouse(state);
    focused = state;
  }

} // namespace axl
