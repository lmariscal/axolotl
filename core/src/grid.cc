#include <axolotl/axolotl.hh>
#include <axolotl/grid.hh>
#include <axolotl/line.hh>
#include <axolotl/shader.hh>
#include <glad.h>

namespace axl {

  constexpr v4 grid_color = v4(0.4f, 0.4f, 0.4f, 1.0f);
  constexpr v4 grid_color_accent = v4(0.5f, 0.5f, 0.5f, 1.0f);
  constexpr v4 grid_color_x_axis = v4(0.0f, 0.0f, 1.0f, 1.0f);
  constexpr v4 grid_color_z_axis = v4(1.0f, 0.0f, 0.0f, 1.0f);

  Grid::Grid(const v2i &size, const v2i &cell_size): _size(size), _cell_size(cell_size) {
    _shader = std::make_unique<Shader>(
      ShaderData(Axolotl::GetDistDir() + "res/shaders/line.vert", Axolotl::GetDistDir() + "res/shaders/line.frag"));

    std::vector<LineVertex> vertices;
    std::vector<v2u> indices;

    for (f32 x = size.x * -0.5f; x < size.x * 0.5f; x += cell_size.x) {
      v4 color = grid_color;
      if (x == 0.0f)
        color = grid_color_x_axis;
      else if (std::fmod(x, cell_size.x * 10.0f) == 0.0f)
        color = grid_color_accent;

      vertices.push_back({ v3(x, 0.0f, size.y * -0.5f), color });
      vertices.push_back({ v3(x, 0.0f, size.y * 0.5f), color });

      indices.push_back(v2u(vertices.size() - 2, vertices.size() - 1));
    }

    for (f32 y = size.y * -0.5f; y < size.y * 0.5f; y += cell_size.y) {
      v4 color = grid_color;
      if (y == 0.0f)
        color = grid_color_z_axis;
      else if (std::fmod(y, cell_size.y * 10.0f) == 0.0f)
        color = grid_color_accent;

      vertices.push_back({ v3(size.x * -0.5f, 0.0f, y), color });
      vertices.push_back({ v3(size.x * 0.5f, 0.0f, y), color });

      indices.push_back(v2(vertices.size() - 2, vertices.size() - 1));
    }

    _line = std::make_unique<Line>(vertices, indices);
  }

  void Grid::Draw(const m4 &view, const m4 &projection) {
    _shader->Bind();
    _shader->SetUniformM4((u32)UniformLocation::ViewMatrix, view);
    _shader->SetUniformM4((u32)UniformLocation::ProjectionMatrix, projection);
    glEnable(GL_LINE_SMOOTH);
    _line->Draw();
  }

} // namespace axl
