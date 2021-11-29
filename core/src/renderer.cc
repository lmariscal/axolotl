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

  Renderer::~Renderer() {

  }

  void Renderer::ClearScreen(const v3 &color) {
    glClearColor(color.x, color.y, color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void Renderer::Resize(u32 width, u32 height) {
    glViewport(0, 0, width, height);
  }

  void Renderer::Render(entt::registry &registry) {
    m4 view(1.0f);
    m4 projection(1.0f);

    Camera *camera = Camera::GetActiveCamera();
    if (camera) {
      view = camera->GetViewMatrix();
      projection = camera->GetProjectionMatrix(*_window);
    }

    auto entities = registry.view<Ento, Model, Material>();
    for (auto entity : entities) {
      Model &model = entities.get<Model>(entity);
      Material &material = entities.get<Material>(entity);
      Ento &ento = entities.get<Ento>(entity);
      if (ento.marked_for_deletion)
        continue;

      m4 model_mat(1.0f);
      Transform *transform = registry.try_get<Transform>(entity);
      if (transform)
        model_mat = transform->GetModelMatrix();

      glEnable(GL_DEPTH_TEST);

      material.GetShader().Bind();
      material.GetShader().SetUniformM4((u32)UniformLocation::ModelMatrix, model_mat);
      material.GetShader().SetUniformM4((u32)UniformLocation::ViewMatrix, view);
      material.GetShader().SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);
      model.Draw();
    }
  }

  void Renderer::SetMeshWireframe(bool state) {
    if (state) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
  }

} // namespace axl
