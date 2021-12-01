#include <axolotl/renderer.h>

#include <axolotl/window.h>
#include <axolotl/model.h>
#include <axolotl/material.h>
#include <axolotl/window.h>
#include <axolotl/camera.h>
#include <axolotl/transform.h>
#include <axolotl/texture.h>
#include <axolotl/ento.h>

#include <glad.h>

namespace axl {

  Renderer::Renderer(Window *window):
    _window(window)
  {
    if (gladLoadGL() != GL_TRUE) {
      log::error("Failed to load OpenGL");
      exit(-1);
      return;
    }
  }

  void Renderer::ClearScreen(const v3 &color) {
    glClearColor(color.x, color.y, color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void Renderer::Resize(u32 width, u32 height) {
    glViewport(0, 0, width, height);
  }

  void Renderer::Render(Scene &scene, bool show_data) {
    m4 view(1.0f);
    m4 projection(1.0f);

    Camera *camera = Camera::GetActiveCamera();
    if (camera) {
      Ento camera_ento = Camera::GetActiveCameraEnto();
      view = camera->GetViewMatrix(camera_ento);
      projection = camera->GetProjectionMatrix(*_window);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    auto entities = scene.GetRegistry().group<Model, Material>();
    for (auto entity : entities) {
      Ento ento = scene.FromHandle(entity);
      Model &model = ento.GetComponent<Model>();
      Material &material = ento.GetComponent<Material>();

      m4 model_mat(1.0f);
      if (ento.HasComponent<Transform>())
        model_mat = ento.GetComponent<Transform>().GetModelMatrix(ento);

      material.GetShader().Bind();
      material.GetShader().SetUniformM4((u32)UniformLocation::ModelMatrix, model_mat);
      material.GetShader().SetUniformM4((u32)UniformLocation::ViewMatrix, view);
      material.GetShader().SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);

      model.Draw(material);
    }

    if (show_data)
      ShowData();
  }

  void Renderer::ShowData() {
    ImGui::Begin("Renderer");

    if (ImGui::CollapsingHeader("General Information", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::Text("  FPS: %u", _fps);
      ImGui::Text("Delta: %.4fms", _delta_time);
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
