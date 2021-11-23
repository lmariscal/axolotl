#include <axolotl/renderer.h>

#include <axolotl/window.h>
#include <axolotl/mesh.h>
#include <axolotl/shader.h>
#include <axolotl/window.h>
#include <axolotl/camera.h>
#include <axolotl/transform.h>
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
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Renderer::Resize(u32 width, u32 height) {
    glViewport(0, 0, width, height);
  }

  void Renderer::Render(entt::registry &registry) {
    Camera *camera = Camera::GetActiveCamera();
    m4 view = camera->GetViewMatrix();
    m4 projection = camera->GetProjectionMatrix(*_window);

    auto entities = registry.view<Mesh, Shader, Ento>();
    for (auto entity : entities) {
      Mesh &mesh = entities.get<Mesh>(entity);
      Shader &shader = entities.get<Shader>(entity);
      Ento &ento = entities.get<Ento>(entity);
      // log::debug("Rendering {}", uuids::to_string(ento.id));

      m4 model(1.0f);
      Transform *transform = registry.try_get<Transform>(entity);
      if (transform) {
        model = transform->GetModelMatrix();
      }

      shader.Bind();
      shader.SetUniformModel(model);
      shader.SetUniformView(view);
      shader.SetUniformProjection(projection);
      shader.SetOthers();
      mesh.Draw();
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
