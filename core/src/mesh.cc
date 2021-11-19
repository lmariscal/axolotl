#include <axolotl/mesh.h>

#include <glad.h>

namespace axl {

  Mesh::Mesh(const std::vector<f32> &vertices):
    _vao(0),
    _vbo(0),
    _ibo(0),
    _num_vertices(vertices.size() / 3)
  {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);

    glBindVertexArray(0);
  }

  Mesh::~Mesh() {
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
    if (_ibo)
      glDeleteBuffers(1, &_ibo);
  }

  void Mesh::Draw() {
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, _num_vertices);
    glBindVertexArray(0);
  }

} // namespace axl
