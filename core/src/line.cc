#include <axolotl/line.hh>
#include <glad.h>

namespace axl {

  Line::Line(v3 pos0, v3 pos1, Color color, f32 thickness, bool loop): thickness(thickness), loop(loop), _vertices() {
    LoadBuffers();

    v4 &c = color.rgba;
    _vertices.push_back({ pos0, c });
    _vertices.push_back({ pos1, c });

    if (_line_thickness_range == v2(0.0f))
      glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, value_ptr(_line_thickness_range));
  }

  Line::Line(const std::vector<LineVertex> &vertices, const std::vector<v2u> &indices, f32 thickness):
    _vertices(vertices),
    _indices(indices),
    thickness(thickness),
    loop(false) {
    LoadBuffers();

    if (_line_thickness_range == v2(0.0f))
      glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, value_ptr(_line_thickness_range));
  }

  Line::Line(const std::vector<LineVertex> &vertices, f32 thickness, bool loop):
    _vertices(vertices),
    thickness(thickness),
    loop(loop) {

    LoadBuffers();

    if (_line_thickness_range == v2(0.0f))
      glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, value_ptr(_line_thickness_range));
  }

  Line::Line(Line &&other) {
    _vertices = std::move(other._vertices);
    _indices = std::move(other._indices);
    thickness = other.thickness;
    loop = other.loop;

    LoadBuffers();
  }

  Line::Line(const Line &other) {
    _vertices = other._vertices;
    _indices = other._indices;
    thickness = other.thickness;
    loop = other.loop;

    LoadBuffers();
  }

  Line &Line::operator=(Line &&other) {
    _vertices = std::move(other._vertices);
    _indices = std::move(other._indices);
    thickness = other.thickness;
    loop = other.loop;

    LoadBuffers();

    return *this;
  }

  Line &Line::operator=(const Line &other) {
    _vertices = other._vertices;
    _indices = other._indices;
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * _vertices.size(), _vertices.data(), GL_STATIC_DRAW);

    if (!_indices.empty()) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(v2) * _indices.size(), _indices.data(), GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void *)(sizeof(v3)));

    glBindVertexArray(0);
  }

  void Line::Draw() const {
    glLineWidth(thickness);

    glBindVertexArray(_vao);

    if (!_indices.empty())
      glDrawElements(GL_LINES, _indices.size() * 2, GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(loop ? GL_LINE_LOOP : GL_LINE_STRIP, 0, _vertices.size());

    glBindVertexArray(0);
  }

} // namespace axl
