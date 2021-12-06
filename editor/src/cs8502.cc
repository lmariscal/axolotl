#include "cs8502.hh"

#include <axolotl/ento.hh>
#include <axolotl/camera.hh>
#include <axolotl/iomanager.hh>
#include <axolotl/window.hh>
#include <axolotl/shader.hh>
#include <axolotl/renderer.hh>
#include <axolotl/texture.hh>
#include <axolotl/model.hh>
#include <axolotl/axolotl.hh>
#include <axolotl/light.hh>

namespace axl {

  i32 camera_pos_index = 0;

  std::vector<v3> camera_positions = {
    v3(49.638, 33.295, 195.432),
    v3(-194.659, 155.662, 192.518),
    v3(-198.735, 153.487, -85.937),
    v3(27.474, 172.577, -295.632),
    v3(47.777, 107.789, -97.468),
    v3(3.227, 71.334, -14.981)
  };

  std::vector<v3> camera_rotations = {
    v3(-139.081, -17.701, 0),
    v3(-35.281, -25.201, 0),
    v3(94.739, -21.181, 0),
    v3(113, -24, 0),
    v3(28.019, -15.001, 0),
  };

  std::vector<v3> light_positions = {
    v3(30.0f, 15.0f, 0.0f),
    v3(-3, 20, 154),
    v3(-45, 107, 84),
    v3(42, 125, 15),
    v3(-82, 95, -74),
    v3(-15, 207, 25),
  };

  void CS8502::Init(Window &window) {
    Ento camera = CreateEntity();
    camera.Tag().value = "Camera";
    Camera &camera_comp = camera.AddComponent<Camera>(camera);
    camera_comp.SetAsActive(camera);
    camera.Transform().SetPosition(camera_positions[0]);
    camera.Transform().SetRotation(camera_rotations[0]);
    camera_comp.UpdateVectors(camera);

    i32 light_count = 0;
    for (const v3 &pos : light_positions) {
      Ento light = CreateEntity();
      light.Tag().value = "Light " + std::to_string(light_count++);
      light.AddComponent<Light>(LightType::Point, vec3(1.0f), 0.6f);

      light.Transform().SetPosition(pos);
    }

    std::array<std::string, (i32)ShaderType::Last> shader_paths;
    shader_paths[(i32)ShaderType::Vertex] = Axolotl::GetDistDir() + "res/shaders/testy.vert";
    shader_paths[(i32)ShaderType::Fragment] = Axolotl::GetDistDir() + "res/shaders/testy.frag";

    Ento stronghold = CreateEntity();
    Model &model = stronghold.AddComponent<Model>(stronghold, Axolotl::GetDistDir() + "res/misc/stronghold.gltf", shader_paths);
    model.Init(stronghold);
    stronghold.Transform().SetScale(v3(0.5f));

    Ento boat = CreateEntity();
    Model &boat_model = boat.AddComponent<Model>(boat, Axolotl::GetDistDir() + "res/misc/boat.gltf", shader_paths);
    boat_model.Init(boat);
    boat.Transform().SetPosition(v3(-4.400, -7.000, 141.300));
    boat.Transform().SetScale(v3(10, 10, 10));
    boat.Transform().SetRotation(v3(5.176, 8.986, 0.573));
    boat_model.two_sided = true;

    window.GetRenderer().SetSkybox(new TextureCube(Axolotl::GetDistDir() + "res/textures/TropicalSunnyDay"));
  }

  void CS8502::Update(Window &window, f64 delta) {
    // Execute scene scripting, per entity or an overall script
    IOManager &io = window.GetIOManager();
    Camera *camera = Camera::GetActiveCamera();
    if (camera) {
      if (window.GetLockMouse()) {
        camera_pos_index = 0;
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
      } else {
        Ento camera_ento = Camera::GetActiveCameraEnto();
        Transform &transform = camera_ento.Transform();
        i32 other_index = camera_pos_index + 1;
        if (other_index >= camera_positions.size())
          other_index = 0;

        if (distance(transform.GetPosition(), camera_positions[other_index]) <= 0.5f)
          camera_pos_index = other_index;

        transform.SetPosition(transform.GetPosition() + normalize(camera_positions[other_index] - transform.GetPosition()));
        quat current_rotation = camera_ento.Transform().GetRotationQuat();
        quat target = quat(radians(camera_rotations[other_index]));

        transform.SetRotation(lerp(current_rotation, target, 0.3f * (f32)delta));
        camera->UpdateVectors(camera_ento);
      }
    }
  }

  void CS8502::Focused(Window &window, bool state) {
    window.LockMouse(state);
    focused = state;
  }

} // namespace axl
