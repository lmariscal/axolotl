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
    Camera *camera = Camera::GetActiveCamera();
    m4 view = camera->GetViewMatrix();
    m4 projection = camera->GetProjectionMatrix(*_window);

    auto entities = registry.view<Ento, Model, Material>();
    for (auto entity : entities) {
      Model &model = entities.get<Model>(entity);
      Material &material = entities.get<Material>(entity);
      Ento &ento = entities.get<Ento>(entity);

      m4 model_mat(1.0f);
      Transform *transform = registry.try_get<Transform>(entity);
      if (transform) {
        model_mat = transform->GetModelMatrix();
      }


      glEnable(GL_DEPTH_TEST);

      material.Bind();
      Texture *texture = registry.try_get<Texture>(entity);
      if (texture)
        material._shader->SetUniformTexture(0, TextureStore::GetRendererTextureID(texture->texture_id));
      material._shader->SetUniformModel(model_mat);
      material._shader->SetUniformView(view);
      material._shader->SetUniformProjection(projection);
      material._shader->SetOthers();
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
