#include <axolotl/axolotl.hh>
#include <axolotl/grid.hh>
#include <axolotl/line.hh>
#include <axolotl/shader.hh>
#include <glad.h>

namespace axl {

  Grid::Grid(const v2i &size, const v2i &cell_size): _size(size), _cell_size(cell_size) {
    _shader = std::make_unique<Shader>(
      ShaderData(Axolotl::GetDistDir() + "res/shaders/line.vert", Axolotl::GetDistDir() + "res/shaders/line.frag"));

    std::vector<v3> vertices;
    std::vector<v2u> indices;

    for (f32 x = size.x * -0.5f; x < size.x * 0.5f; x += cell_size.x) {
      vertices.push_back(v3(x, 0.0f, size.y * -0.5f));
      vertices.push_back(v3(x, 0.0f, size.y * 0.5f));

      indices.push_back(v2(vertices.size() - 2, vertices.size() - 1));
    }

    for (f32 y = size.y * -0.5f; y < size.y * 0.5f; y += cell_size.y) {
      vertices.push_back(v3(size.x * -0.5f, 0.0f, y));
      vertices.push_back(v3(size.x * 0.5f, 0.0f, y));

      indices.push_back(v2(vertices.size() - 2, vertices.size() - 1));
    }

    _line = std::make_unique<Line>(vertices, indices);
  }

  void Grid::Draw(const m4 &view, const m4 &projection) {
    _shader->Bind();
    _shader->SetUniformM4((u32)UniformLocation::ViewMatrix, view);
    _shader->SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);
    glEnable(GL_LINE_SMOOTH);
    _shader->SetUniformV4("color", _line->color);
    _line->Draw();
  }

} // namespace axl
