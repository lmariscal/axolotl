#include <algorithm>
#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/framebuffer.hh>
#include <axolotl/grid.hh>
#include <axolotl/material.hh>
#include <axolotl/model.hh>
#include <axolotl/renderer.hh>
#include <axolotl/texture.hh>
#include <axolotl/transform.hh>
#include <axolotl/window.hh>
#include <glad.h>

namespace axl {

  class Renderable {
   public:
    Ento ento;
    Model *model;
    // Material *material;
    Transform *transform;
  };

  Renderer::Renderer(Window *window):
    _window(window),
    _skybox_texture(nullptr),
    _skybox_mesh(nullptr),
    _skybox_shader(nullptr),
    _size(1920, 1080),
    _ambient_light(LightType::Ambient, v3(0.6f), 0.4f),
    _directional_light(LightType::Directional, v3(1.0f), 0.4f),
    _directional_light_direction(v3(0.3f, 0.2f, 0.3f)),
    _show_wireframe(false),
    _show_grid(true) {

    if (gladLoadGL() != GL_TRUE) {
      log::error("Failed to load OpenGL");
      exit(-1);
      return;
    }

    glGenBuffers(1, &_lights_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, _lights_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, (sizeof(LightData) * LIGHT_COUNT) + 32, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _post_process_framebuffer = new FrameBuffer(_size.x, _size.y);

    _line_shader = std::make_unique<Shader>(
      ShaderData(Axolotl::GetDistDir() + "res/shaders/line.vert", Axolotl::GetDistDir() + "res/shaders/line.frag"));
    _grid = std::make_unique<Grid>(v2i(100, 100), v2i(1));

    Mesh::CreateQuad(&_quad_mesh);
    _post_process_shader = new Shader(
      ShaderData(Axolotl::GetDistDir() + "res/shaders/post.vert", Axolotl::GetDistDir() + "res/shaders/post.frag"));
  }

  Renderer::~Renderer() {
    if (_skybox_texture) {
      delete _skybox_texture;
      delete _skybox_mesh;
      delete _skybox_shader;
    }

    delete _quad_mesh;
    delete _post_process_shader;
    delete _post_process_framebuffer;

    glDeleteBuffers(1, &_lights_uniform_buffer);
  }

  const RendererPerformance &Renderer::GetPerformance() const {
    return _last_performance;
  }

  void Renderer::ClearScreen(const v3 &color) {
    glClearColor(color.x, color.y, color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void Renderer::Resize(u32 width, u32 height) {
    glViewport(0, 0, width, height);
    _size = v2i(width, height);
  }

  void Renderer::SetSkybox(TextureCube *skybox) {
    if (_skybox_texture) {
      delete _skybox_texture;
      delete _skybox_mesh;
      delete _skybox_shader;

      _skybox_texture = nullptr;
      _skybox_mesh = nullptr;
      _skybox_shader = nullptr;
    }

    if (!skybox)
      return;

    _skybox_texture = skybox;
    Mesh::CreateCube(&_skybox_mesh);
    _skybox_shader = new Shader(
      ShaderData(Axolotl::GetDistDir() + "res/shaders/skybox.vert", Axolotl::GetDistDir() + "res/shaders/skybox.frag"));
  }

  void Renderer::AddLine(const LinePrimitive &line) {
    _lines.emplace(line);
  }

  void Renderer::Render(Scene &scene, bool show_data, bool focused, Camera &camera, Transform &camera_transform) {
    f64 cpu_starttime = Window::GetTime();
    m4 view(1.0f);
    m4 projection(1.0f);

    u32 gl_starttime;
    u32 gl_endtime;
    glGenQueries(1, &gl_starttime);
    glGenQueries(1, &gl_endtime);
    glQueryCounter(gl_starttime, GL_TIMESTAMP);

    _performance.StartCapture(_window->GetTime());

    view = camera.GetViewMatrix(&camera_transform);
    projection = camera.GetProjectionMatrix(*_window);

    f64 orginzation_starttime = Window::GetTime();

    entt::registry &registry = scene.GetRegistry();
    std::vector<Renderable> renderables;

    auto entities = registry.view<Model>();
    for (auto entity : entities) {
      Ento ento = scene.FromHandle(entity);
      Model &model = ento.GetComponent<Model>();
      // Material &material = ento.GetComponent<Material>();
      Transform &transform = ento.GetComponent<Transform>();

      Renderable renderable;
      renderable.ento = ento;
      renderable.model = &model;
      // renderable.material = (*model._materials).begin()->second.get();
      renderable.transform = &transform;
      renderables.push_back(renderable);

      _performance.mesh_count += model._meshes->size();
      for (auto &mesh : *model._meshes) {
        _performance.vertex_count += mesh->_num_vertices;
        _performance.triangle_count += mesh->_num_indices / 3;
      }
    }
    _performance.renderables = renderables.size();

    std::sort(renderables.begin(), renderables.end(), [](const Renderable &a, const Renderable &b) {
      return a.transform->GetPosition().z < b.transform->GetPosition().z;
    });

    f64 orginzation_endtime = Window::GetTime();
    _performance.organization_time_accum += orginzation_endtime - orginzation_starttime;

    f64 lights_starttime = Window::GetTime();
    std::vector<LightData> lights_data;

    LightData ambient_light_data;
    ambient_light_data.position = v4(1.0f);
    ambient_light_data.color = v4(_ambient_light.color);
    ambient_light_data.intensity = _ambient_light.intensity;
    lights_data.push_back(ambient_light_data);

    LightData directional_light_data;
    directional_light_data.position = v4(_directional_light_direction, 0.0f);
    directional_light_data.color = v4(_directional_light.color);
    directional_light_data.intensity = _directional_light.intensity;
    lights_data.push_back(directional_light_data);

    auto lights = registry.view<Light>();
    for (auto entity : lights) {
      Ento light_ento = scene.FromHandle(entity);
      Transform &transform = light_ento.Transform();
      Light &light = light_ento.GetComponent<Light>();

      LightData light_data;
      v4 position = v4(transform.GetPosition(), 1.0f);
      if (light_ento.HasParent())
        position = transform.GetModelMatrix() * v4(0.0f, 0.0f, 0.0f, 1.0f);
      light_data.position = position;
      light_data.color = light.color;
      light_data.intensity = light.intensity;

      if (lights.size() >= LIGHT_COUNT) {
        log::warn("Too many lights");
        break;
      }
      lights_data.push_back(light_data);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, _lights_uniform_buffer);
    i32 size = lights_data.size();

    glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &size);

    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(v3), value_ptr(camera_transform.GetPosition()));

    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(LightData) * lights_data.size(), lights_data.data());
    glBufferSubData(GL_UNIFORM_BUFFER,
                    sizeof(LightData) * lights_data.size() + 32,
                    sizeof(LightData) * (LIGHT_COUNT - lights_data.size()),
                    nullptr);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    f64 lights_endtime = Window::GetTime();
    _performance.lights_time_accum += lights_endtime - lights_starttime;

    f64 main_draw_starttime = Window::GetTime();
    _post_process_framebuffer->Bind();

    ClearScreen(v3(0.3f));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (_show_wireframe) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    for (auto entity : renderables) {
      m4 model_mat = entity.transform->GetModelMatrix();

      // TODO: This is horrible, MAKE A RESOURCE MANAGER!
      for (auto itr = entity.model->_materials->begin(); itr != entity.model->_materials->end(); ++itr) {
        Material &material = *itr->second;
        material.GetShader().Bind();
        u32 block_index = material.GetShader().GetUniformBlockIndex("Lights");
        material.GetShader().SetUniformBlockBinding(block_index, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _lights_uniform_buffer);

        material.GetShader().SetUniformM4((u32)UniformLocation::ModelMatrix, model_mat);
        material.GetShader().SetUniformM4((u32)UniformLocation::ViewMatrix, view);
        material.GetShader().SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);
      }

      entity.model->Draw();
    }

    if (_show_grid)
      _grid->Draw(view, projection);

    if (_skybox_texture) {
      glDisable(GL_CULL_FACE);
      glDepthFunc(GL_LEQUAL);
      _skybox_shader->Bind();
      mat4 skybox_view = mat4(mat3(view));
      _skybox_shader->SetUniformM4((u32)UniformLocation::ViewMatrix, skybox_view);
      _skybox_shader->SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);

      _skybox_texture->Bind();
      _skybox_mesh->Draw();
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Debug draw
    _line_shader->Bind();
    _line_shader->SetUniformM4((u32)UniformLocation::ViewMatrix, view);
    _line_shader->SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);
    while (!_lines.empty()) {
      LinePrimitive &line = _lines.front();
      line.Draw();
      _lines.pop();
    }

    _post_process_framebuffer->Unbind();

    f64 main_draw_endtime = Window::GetTime();
    _performance.main_draw_time_accum += main_draw_endtime - main_draw_starttime;

    // Post process
    f64 post_draw_starttime = Window::GetTime();

    _post_process_shader->Bind();
    _post_process_shader->SetUniformM4((u32)UniformLocation::ModelMatrix, m4(1.0f));
    _post_process_shader->SetUniformM4((u32)UniformLocation::ViewMatrix, m4(1.0f));
    _post_process_shader->SetUniformM4((u32)UniformLocation::ProjectionMatrix, m4(1.0f));
    _post_process_framebuffer->GetTexture(FrameBufferTexture::Color).Bind(1);
    glUniform1i(_post_process_shader->GetUniformLocation("tex"), 1);
    _post_process_framebuffer->GetTexture(FrameBufferTexture::DepthStencil).Bind(2);
    glUniform1i(_post_process_shader->GetUniformLocation("depth_tex"), 2);

    v2 relative_mouse = v2(0.0f);
    if (focused)
      relative_mouse = _window->GetIOManager().GetRelativePosition();
    glUniform2f(_post_process_shader->GetUniformLocation("mouse_delta"), relative_mouse.x, relative_mouse.y);

    v4 viewport_size;
    glGetFloatv(GL_VIEWPORT, value_ptr(viewport_size));
    glUniform2f(_post_process_shader->GetUniformLocation("viewport_size"), viewport_size.z, viewport_size.w);

    _quad_mesh->Draw();

    f64 post_draw_endtime = Window::GetTime();
    _performance.post_draw_time_accum += post_draw_endtime - post_draw_starttime;

    glQueryCounter(gl_endtime, GL_TIMESTAMP);
    i32 query_available = false;
    while (!query_available) {
      glGetQueryObjectiv(gl_endtime, GL_QUERY_RESULT_AVAILABLE, &query_available);
    }

    u64 start_time, end_time;
    glGetQueryObjectui64v(gl_starttime, GL_QUERY_RESULT, &start_time);
    glGetQueryObjectui64v(gl_endtime, GL_QUERY_RESULT, &end_time);

    _performance.gpu_render_time_accum += (f64)(end_time - start_time) / 1000000.0;

    f64 cpu_endtime = Window::GetTime();
    _performance.cpu_render_time_accum += cpu_endtime - cpu_starttime;

    _performance.EndCapture(_window->GetTime(), _window->GetDeltaTime());
    _last_performance = _performance;
    if (show_data)
      ShowData();
  }

  void RendererPerformance::StartCapture(f64 now) {
    mesh_count = 0;
    vertex_count = 0;
    triangle_count = 0;
    Mesh::_draw_calls = 0;
  }

  void RendererPerformance::EndCapture(f64 now, f64 delta) {
    if (now - last_time >= 0.25) {
      last_time = now;

      fps = frame_count * 4;

      delta_time = delta_time_accum / frame_count;
      delta_time_accum = 0.0;

      gpu_render_time = gpu_render_time_accum / frame_count;
      gpu_render_time_accum = 0.0;

      cpu_render_time = cpu_render_time_accum / frame_count;
      cpu_render_time_accum = 0.0;

      organization_time = organization_time_accum / frame_count;
      organization_time_accum = 0.0;

      main_draw_time = main_draw_time_accum / frame_count;
      main_draw_time_accum = 0.0;

      post_draw_time = post_draw_time_accum / frame_count;
      post_draw_time_accum = 0.0;

      lights_time = lights_time_accum / frame_count;
      lights_time_accum = 0.0;

      frame_count = 0;
    }

    draw_calls = Mesh::_draw_calls;
    delta_time_accum += delta;
    frame_count++;
  }

  void Renderer::ShowData() {
    ImGui::Begin("Renderer");
    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
      axl::ShowData("Ambient Light", _ambient_light.color);
      axl::ShowData("Ambient Light Intensity", _ambient_light.intensity);

      axl::ShowData("Directional Light", _directional_light.color);
      axl::ShowData("Directional Light Intensity", _directional_light.intensity);
      axl::ShowData("Directional Light Angle", _directional_light_direction);
    }

    ImGui::End();
  }

  void Renderer::SetMeshWireframe(bool state) {
    _show_wireframe = state;
  }

  void Renderer::SetShowGrid(bool state) {
    _show_grid = state;
  }

} // namespace axl
