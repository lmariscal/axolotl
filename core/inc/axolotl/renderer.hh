#pragma once

#include <axolotl/light.hh>
#include <axolotl/line.hh>
#include <axolotl/scene.hh>
#include <axolotl/types.hh>
#include <entt/entt.hpp>
#include <queue>

namespace axl {

  class Window;
  class GUI;

  class TextureCube;
  class Grid;
  class Mesh;
  class Camera;
  class Transform;
  class Shader;
  class FrameBuffer;

  class RendererPerformance {
   public:
    f64 organization_time;
    f64 organization_time_accum;
    f64 main_draw_time;
    f64 main_draw_time_accum;
    f64 post_draw_time;
    f64 post_draw_time_accum;
    f64 lights_time;
    f64 lights_time_accum;
    f64 gpu_render_time;
    f64 gpu_render_time_accum;
    f64 cpu_render_time;
    f64 cpu_render_time_accum;
    f64 delta_time;
    f64 delta_time_accum;
    f64 last_time;
    u32 fps;
    u32 frame_count;
    u32 renderables;
    u32 mesh_count;
    u32 vertex_count;
    u32 triangle_count;
    u32 draw_calls;

    void StartCapture(f64 now);
    void EndCapture(f64 now, f64 delta);
  };

  class Renderer {
   public:
    Renderer(Window *window);
    ~Renderer();

    void ClearScreen(const v3 &color);
    void Resize(u32 width, u32 height);
    void SetSkybox(TextureCube *skybox);

    void Render(Scene &scene, bool show_data, bool focused, Camera &camera, Transform &camera_transform);
    void SetMeshWireframe(bool state);
    void SetShowGrid(bool state);
    void AddLine(const LinePrimitive &line);

    const RendererPerformance &GetPerformance() const;

   protected:
    friend class GUI;

    void ShowData();

    RendererPerformance _performance;
    RendererPerformance _last_performance;

    std::unique_ptr<Shader> _line_shader;
    std::queue<LinePrimitive> _lines;

    std::unique_ptr<Grid> _grid;

    bool _show_wireframe;
    bool _show_grid;
    v2i _size;

    u32 _lights_uniform_buffer;

    Light _ambient_light;
    Light _directional_light;
    v3 _directional_light_direction;

    Window *_window;

    TextureCube *_skybox_texture;
    Mesh *_skybox_mesh;
    Shader *_skybox_shader;

    Mesh *_quad_mesh;
    Shader *_post_process_shader;
    FrameBuffer *_post_process_framebuffer;
  };

} // namespace axl
