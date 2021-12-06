#include <axolotl/renderer.hh>

#include <axolotl/window.hh>
#include <axolotl/model.hh>
#include <axolotl/material.hh>
#include <axolotl/window.hh>
#include <axolotl/camera.hh>
#include <axolotl/transform.hh>
#include <axolotl/texture.hh>
#include <axolotl/ento.hh>
#include <axolotl/axolotl.hh>
#include <axolotl/framebuffer.hh>

#include <glad.h>

namespace axl {

  Renderer::Renderer(Window *window):
    _window(window),
    _skybox_texture(nullptr),
    _skybox_mesh(nullptr),
    _skybox_shader(nullptr),
    _size(1920, 1080),
    ambient_light(LightType::Ambient, v3(0.3f), 0.3f)
  {
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

    Mesh::CreateQuad(&_quad_mesh);
    _post_process_shader = new Shader(ShaderData(Axolotl::GetDistDir() + "res/shaders/post.vert", Axolotl::GetDistDir() + "res/shaders/post.frag"));
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
    _skybox_shader = new Shader(ShaderData(
        Axolotl::GetDistDir() + "res/shaders/skybox.vert",
        Axolotl::GetDistDir() + "res/shaders/skybox.frag"
      ));
  }

  void Renderer::Render(Scene &scene, bool show_data, bool focused) {
    m4 view(1.0f);
    m4 projection(1.0f);

    Camera *camera = Camera::GetActiveCamera();
    if (camera) {
      Ento camera_ento = Camera::GetActiveCameraEnto();
      view = camera->GetViewMatrix(camera_ento);
      projection = camera->GetProjectionMatrix(*_window);
    }

    entt::registry &registry = scene.GetRegistry();

    std::vector<LightData> lights_data;

    LightData ambient_light_data;
    ambient_light_data.position = v4(1.0f);
    ambient_light_data.color = v4(ambient_light.color);
    ambient_light_data.intensity = ambient_light.intensity;
    lights_data.push_back(ambient_light_data);

    auto lights = registry.view<Light>();
    for (auto entity: lights) {
      Transform &transform = registry.get<Transform>(entity);
      Light &light = registry.get<Light>(entity);

      LightData light_data;
      light_data.position = vec4(transform.GetPosition(), 1.0f);
      light_data.color = light.color;
      light_data.intensity = light.intensity;

      lights_data.push_back(light_data);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, _lights_uniform_buffer);
    i32 size = lights_data.size();

    glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &size);

    if (camera) {
      Ento camera_ento = Camera::GetActiveCameraEnto();
      Transform &transform = registry.get<Transform>(camera_ento);
      glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(v3), value_ptr(transform.GetPosition()));
    }

    glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(LightData) * lights_data.size(), lights_data.data());
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(LightData) * lights_data.size() + 32, sizeof(LightData) * (LIGHT_COUNT - lights_data.size()), nullptr);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    _post_process_framebuffer->Bind();

    ClearScreen(v3(0.3f));

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    auto entities = registry.group<Model, Material>();
    for (auto entity : entities) {
      Ento ento = scene.FromHandle(entity);
      Model &model = ento.GetComponent<Model>();
      Material &material = ento.GetComponent<Material>();

      m4 model_mat(1.0f);
      if (ento.HasComponent<Transform>())
        model_mat = ento.GetComponent<Transform>().GetModelMatrix(ento);

      material.GetShader().Bind();
      u32 block_index = material.GetShader().GetUniformBlockIndex("Lights");
      material.GetShader().SetUniformBlockBinding(block_index, 0);
      glBindBufferBase(GL_UNIFORM_BUFFER, 0, _lights_uniform_buffer);

      material.GetShader().SetUniformM4((u32)UniformLocation::ModelMatrix, model_mat);
      material.GetShader().SetUniformM4((u32)UniformLocation::ViewMatrix, view);
      material.GetShader().SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);

      model.Draw(material);
    }

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

    _post_process_framebuffer->Unbind();

    // Post process

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

    if (show_data)
      ShowData();
  }

  void Renderer::ShowData() {
    ImGui::Begin("Renderer");

    if (ImGui::CollapsingHeader("General Information", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("  FPS: %u", _fps);
      ImGui::Text("Delta: %.2fms", _delta_time * 1000.0);
    }

    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
      ShowDataColor("Ambient Light", ambient_light.color);
      axl::ShowData("Ambient Light Intensity", ambient_light.intensity);
    }

    f64 now = _window->GetTime();
    if (now - _last_time >= 0.25) {
      _last_time = now;

      _fps = _frame_count * 4;
      _delta_time = _delta_time_accum / _frame_count;

      _frame_count = 0;
      _delta_time_accum = 0.0;
    }

    _delta_time_accum += _window->GetDeltaTime();
    _frame_count++;

    ImGui::End();
  }

  void Renderer::SetMeshWireframe(bool state) {
    if (state) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

} // namespace axl
