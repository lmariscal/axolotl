#include "test_scene.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/geometry.hh>
#include <axolotl/line.hh>
#include <axolotl/material.hh>
#include <axolotl/model.hh>
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

    Ento cube_ento = CreateEntity();
    cube_id = cube_ento.id;
    cube_ento.Tag().value = "Cube";
    std::vector<std::string> shader_paths = { Axolotl::GetDistDir() + "res/shaders/testy.vert",
                                              Axolotl::GetDistDir() + "res/shaders/testy.frag" };
    Model &cube_model = cube_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
    TextureStore::ProcessQueue();

    Ento cube_other_ento = CreateEntity();
    cube_other_id = cube_other_ento.id;
    cube_other_ento.Tag().value = "CubeOther";
    cube_other_ento.Transform().SetPosition(v3(3.0f, 0.0f, 0.0f));

    Model &cube_other_model =
      cube_other_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
    TextureStore::ProcessQueue();

    Ento sphere_ento = CreateEntity();
    sphere_id = sphere_ento.id;
    sphere_ento.Tag().value = "Sphere";
    sphere_ento.Transform().SetPosition(v3(-3.0f, 0.0f, 0.0f));
    sphere_ento.Transform().SetScale(v3(0.5f));
    Model &sphere_model = sphere_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Sphere.fbx", shader_paths);
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

      if (io.PadPresent(Pad::Pad0)) {
        if (io.ButtonDown(Pad::Pad0, PadButton::LeftBumper))
          camera->MoveCamera(CameraDirection::Down, delta);
        if (io.ButtonDown(Pad::Pad0, PadButton::RightBumper))
          camera->MoveCamera(CameraDirection::Up, delta);

        constexpr f32 AXIS_DEAD_ZONE = 0.01f;

        f32 left_x = io.GetAxis(Pad::Pad0, JoyStick::LeftX);
        f32 left_y = io.GetAxis(Pad::Pad0, JoyStick::LeftY);

        if (left_x > AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Right, delta * left_x);
        if (left_x < -AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Left, delta * -left_x);

        if (left_y > AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Back, delta * left_y);
        if (left_y < -AXIS_DEAD_ZONE)
          camera->MoveCamera(CameraDirection::Front, delta * -left_y);

        // --------------------------------------------------------------------------------

        f32 right_x = io.GetAxis(Pad::Pad0, JoyStick::RightX);
        f32 right_y = io.GetAxis(Pad::Pad0, JoyStick::RightY);

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

    v3 start = v3(-6.0f, 3.0f, -6.0f);
    Ray ray(start);
    LinePrimitive line(start, start + v3(0.0f, 0.0f, 100.0f));
    window.GetRenderer().AddLine(line);

    Ento cube1 = FromID(cube_id);
    Ento cube2 = FromID(cube_other_id);
    Ento sphere = FromID(sphere_id);
    AABB cube1_aabb(cube1.Transform().GetPosition(), cube1.Transform().GetScale());
    // AABB cube2_aabb(cube2.Transform().GetPosition(), cube2.Transform().GetScale());

    // if (cube1_aabb.AABBInside(cube2_aabb)) {
    //   log::debug("Collision!");
    // }

    // OBB cube1_obb(cube1.Transform().GetPosition(), cube1.Transform().GetScale(), cube1.Transform().GetRotation());
    OBB cube2_obb(cube2.Transform().GetPosition(), cube2.Transform().GetScale(), cube2.Transform().GetRotation());
    Sphere sphere_sphere(sphere.Transform().GetPosition(), sphere.Transform().GetScale().x);

    if (cube1_aabb.OBBInside(cube2_obb)) {
      log::debug("Collision!");
    }

    if (sphere_sphere.OBBInside(cube2_obb)) {
      log::debug("Collision! Sphere OBB");
    }

    if (sphere_sphere.AABBInside(cube1_aabb)) {
      log::debug("Collision! Sphere AABB");
    }

    if (cube1_aabb.RayInside(ray) >= 0.0f) {
      log::debug("Collision! Ray AABB");
    }

    if (sphere_sphere.RayInside(ray) >= 0.0f) {
      log::debug("Collision! Ray Sphere");
    }

    if (cube2_obb.RayInside(ray) >= 0.0f) {
      log::debug("Collision! Ray OBB");
    }
  }

  void TestScene::Focused(Window &window, bool state) {
    window.LockMouse(state);
    focused = state;
  }

} // namespace axl
