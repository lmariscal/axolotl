#pragma once

#include <axolotl/types.hh>

#include <entt/entt.hpp>

#include <axolotl/scene.hh>
#include <axolotl/light.hh>

namespace axl {

  class Window;
  class GUI;

  struct TextureCube;
  struct Mesh;
  struct Shader;

  class Renderer {
   public:
    Renderer(Window *window);
    ~Renderer();

    void ClearScreen(const v3 &color);
    void Resize(u32 width, u32 height);
    void SetSkybox(TextureCube *skybox);

    void Render(Scene &scene, bool show_data);
    void SetMeshWireframe(bool state);

   protected:
    friend class GUI;

    void ShowData();

    v2i _size;
    f64 _delta_time;
    f64 _delta_time_accum;
    f64 _last_time;
    u32 _fps;
    u32 _frame_count;

    u32 _lights_uniform_buffer;

    Light ambient_light;

    Window *_window;

    TextureCube *_skybox_texture;
    Mesh *_skybox_mesh;
    Shader *_skybox_shader;

    Mesh *_quad_mesh;
    Shader *_post_process_shader;
  };

} // namespace axl
