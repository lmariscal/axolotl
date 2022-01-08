#include "level2.hh"

#include "menu.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/model.hh>
#include <axolotl/physics.hh>
#include <axolotl/renderer.hh>
#include <axolotl/texture.hh>
#include <axolotl/window.hh>
#include <fstream>

namespace axl {

  void AILevel::AddWall(const v2i &pos, const std::vector<std::string> &shader_paths) {
    // Greedy Meshing, anyone? No? Okay, let's do it the unoptimized way.
    // Or at least, you know, make it one mesh for the long walls.

    Ento wall_ento = CreateEntity();
    wall_ento.Tag().value = "Wall";
    wall_ento.AddComponent<OBBCollider>(wall_ento.Transform().GetPosition(), v3(1.0f));
    wall_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_transform = wall_ento.Transform();
    wall_transform.SetPosition(v3(pos.x, 4.0f, pos.y));
    wall_transform.SetScale(v3(1.0f, 3.0f, 1.0f));

    Model &wall_model = wall_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);
  }

  void AILevel::GenerateMazeFromFile() {
    std::vector<std::string> shader_paths = { Axolotl::GetDistDir() + "res/shaders/testy.vert",
                                              Axolotl::GetDistDir() + "res/shaders/testy.frag" };

    std::ifstream file(Axolotl::GetDistDir() + "res/misc/maze.txt");
    if (!file.is_open()) {
      log::error("could not open maze file");
      return;
    }

    std::string line;
    std::getline(file, line);
    i32 width = std::stoi(line);
    std::getline(file, line);
    i32 height = std::stoi(line);

    log::info("maze size: {}x{}", width, height);

    i32 z = 0;
    while (std::getline(file, line)) {
      if (z == 0 || z == height - 1) {
        z++;
        continue;
      }

      i32 x = 0;
      for (char c : line) {
        if (x == 0 || x == width - 1) {
          x++;
          continue;
        }

        if (c == '#')
          AddWall({ x * 2, z * 2 }, shader_paths);
        x++;
      }
      z++;
    }

    Ento floor_ento = CreateEntity();
    floor_ento.Tag().value = "Floor";
    floor_ento.AddComponent<OBBCollider>(floor_ento.Transform().GetPosition(), v3(1.0f));
    floor_ento.AddComponent<RigidBody>(0.0);
    Transform &floor_transform = floor_ento.Transform();
    floor_transform.SetPosition(v3(width - 1, 0, height - 1));
    floor_transform.SetScale(v3(width, 1.0f, height));
    Model &wall_model = floor_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_b_ento = CreateEntity();
    wall_b_ento.Tag().value = "Wall B";
    wall_b_ento.AddComponent<OBBCollider>(wall_b_ento.Transform().GetPosition(), v3(1.0f));
    wall_b_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_b_transform = wall_b_ento.Transform();
    wall_b_transform.SetPosition(v3(width - 1, 4.0f, (height - 1) * 2.0f));
    wall_b_transform.SetScale(v3(width, 3.0f, 1.0f));
    Model &wall_b_model = wall_b_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_c_ento = CreateEntity();
    wall_c_ento.Tag().value = "Wall C";
    wall_c_ento.AddComponent<OBBCollider>(wall_c_ento.Transform().GetPosition(), v3(1.0f));
    wall_c_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_c_transform = wall_c_ento.Transform();
    wall_c_transform.SetPosition(v3((width - 1) * 2.0f, 4.0f, height - 1));
    wall_c_transform.SetScale(v3(1.0f, 3.0f, height));
    Model &wall_c_model = wall_c_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_a_ento = CreateEntity();
    wall_a_ento.Tag().value = "Wall A";
    wall_a_ento.AddComponent<OBBCollider>(wall_a_ento.Transform().GetPosition(), v3(1.0f));
    wall_a_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_a_transform = wall_a_ento.Transform();
    wall_a_transform.SetPosition(v3(0, 4.0f, height - 1));
    wall_a_transform.SetScale(v3(1.0f, 3.0f, height));
    Model &wall_a_model = wall_a_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    Ento wall_d_ento = CreateEntity();
    wall_d_ento.Tag().value = "Wall D";
    wall_d_ento.AddComponent<OBBCollider>(wall_d_ento.Transform().GetPosition(), v3(1.0f));
    wall_d_ento.AddComponent<RigidBody>(0.0);
    Transform &wall_d_transform = wall_d_ento.Transform();
    wall_d_transform.SetPosition(v3(width - 1, 4.0f, 0));
    wall_d_transform.SetScale(v3(width, 3.0f, 1.0f));
    Model &wall_d_model = wall_d_ento.AddComponent<Model>(Axolotl::GetDistDir() + "res/misc/Cube.fbx", shader_paths);

    TextureStore::ProcessQueue();
    file.close();
  }

  void AILevel::Init(Window &window) {
    Ento camera_ento = CreateEntity();
    camera_ento.Tag().value = "Camera";
    Camera &camera_component = camera_ento.AddComponent<Camera>();
    camera_component.SetAsActive();

    window.GetRenderer().SetShowGrid(false);

    GenerateMazeFromFile();
  }

  void AILevel::Update(Window &window, f64 delta) { }

  void AILevel::UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    MenuLevel::ShowBackMenu(window, frame_size, frame_pos, _show_menu);
  }

  void AILevel::Focused(Window &window, bool state) { }

} // namespace axl
