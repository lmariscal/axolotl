#include <axolotl/renderer.h>

#include <axolotl/window.h>
#include <axolotl/mesh.h>
#include <axolotl/shader.h>
#include <axolotl/window.h>
#include <axolotl/camera.h>
#include <axolotl/transform.h>

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
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Renderer::Resize(u32 width, u32 height) {
    glViewport(0, 0, width, height);
  }

  void Renderer::Render(entt::registry &registry) {
    Camera *camera = Camera::GetActiveCamera();
    camera->SetPerspective();
    m4 view = camera->GetViewMatrix();
    m4 projection = camera->GetProjectionMatrix(*_window);

    auto entities = registry.view<Mesh, Shader>();
    for (auto entity : entities) {
      Mesh &mesh = entities.get<Mesh>(entity);
      Shader &shader = entities.get<Shader>(entity);

      m4 model(1.0f);
      Transform *transform = registry.try_get<Transform>(entity);
      if (transform) {
        model *= toMat4(transform->GetRotation());
        model = scale(model, transform->GetScale());
        model = translate(model, transform->GetPosition());
      }

      shader.Bind();
      shader.SetUniformM4("uModel", model);
      shader.SetUniformM4("uView", view);
      shader.SetUniformM4("uProjection", projection);
      mesh.Draw();
    }
  }

} // namespace axl
