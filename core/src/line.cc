#include <axolotl/line.hh>
#include <glad.h>

namespace axl {

  Line::Line(v3 pos0, v3 pos1, Color color, f32 thickness, bool loop):
    _vertices({ pos0, pos1 }),
    color(color),
    thickness(thickness),
    loop(loop) {
    LoadBuffers();

    if (_line_thickness_range == v2(0.0f))
      glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, value_ptr(_line_thickness_range));
  }

  Line::Line(const std::vector<v3> &vertices, const std::vector<v2u> &indices, Color color, f32 thickness):
    _vertices(vertices),
    _indices(indices),
    color(color),
    thickness(thickness),
    loop(false) {
    LoadBuffers();

    if (_line_thickness_range == v2(0.0f))
      glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, value_ptr(_line_thickness_range));
  }

  Line::Line(const std::vector<v3> &positions, Color color, f32 thickness, bool loop):
    _vertices(positions),
    color(color),
    thickness(thickness),
    loop(loop) {
    LoadBuffers();

    if (_line_thickness_range == v2(0.0f))
      glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, value_ptr(_line_thickness_range));
  }

  Line::Line(Line &&other) {
    _vertices = std::move(other._vertices);
    color = other.color;
    thickness = other.thickness;
    loop = other.loop;

    LoadBuffers();
  }

  Line::Line(const Line &other) {
    _vertices = other._vertices;
    color = other.color;
    thickness = other.thickness;
    loop = other.loop;

    LoadBuffers();
  }

  Line &Line::operator=(Line &&other) {
    _vertices = std::move(other._vertices);
    color = other.color;
    thickness = other.thickness;
    loop = other.loop;

    LoadBuffers();

    return *this;
  }

  Line &Line::operator=(const Line &other) {
    _vertices = other._vertices;
    color = other.color;
    thickness = other.thickness;
    loop = other.loop;

    LoadBuffers();

    return *this;
  }

  Line::~Line() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
  }

  void Line::LoadBuffers() {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(v3) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

    if (!_indices.empty()) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(v2) * _indices.size(), _indices.data(), GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(v3), (void *)0);

    glBindVertexArray(0);
  }

  void Line::Draw() const {
    glLineWidth(thickness);

    glBindVertexArray(_vao);

    if (!_indices.empty())
      glDrawElements(GL_LINES, _indices.size(), GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(loop ? GL_LINE_LOOP : GL_LINE_STRIP, 0, _vertices.size());

    glBindVertexArray(0);
  }

} // namespace axl
