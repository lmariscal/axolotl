#include <axolotl/mesh.hh>

#include <glad.h>

namespace axl {

  Mesh::Mesh(const std::vector<f32> &vertices, const std::vector<u32> &indices):
    _vao(0),
    _num_vertices(0),
    _num_indices(0),
    _single_mesh(true)
  {
    _num_vertices = vertices.size() / 11;
    _num_indices = indices.size();

    LoadBuffers(vertices, indices);
  }

  void Mesh::LoadBuffers(const std::vector<f32> &vertices, const std::vector<u32> &indices) {
    log::debug("Creating mesh with {} vertices and {} indices", _num_vertices, _num_indices);
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    u32 vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);
    _buffers.push_back({ vbo, GL_ARRAY_BUFFER });

    if (indices.size() > 0) {
      u32 ibo = 0;
      glGenBuffers(1, &ibo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
      _buffers.push_back({ ibo, GL_ELEMENT_ARRAY_BUFFER });
    }

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)0);
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(3 * sizeof(f32)));
    // tangents
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(6 * sizeof(f32)));
    // texture coords
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(f32), (void*)(9 * sizeof(f32)));

    glBindVertexArray(0);

    log::debug("Mesh created {}", _vao);
  }

  Mesh::~Mesh() {
    if ( 1)
      return;
    log::debug("Deleting mesh {}", _vao);
    for (auto &buffer : _buffers)
      glDeleteBuffers(1, &buffer.vbo);
    glDeleteVertexArrays(1, &_vao);
  }

  void Mesh::SetMaterialID(u32 id) {
    _material_id = id;
  }

  u32 Mesh::GetMaterialID() const {
    return _material_id;
  }

  void Mesh::Draw() {
    glBindVertexArray(_vao);
    if (_num_indices > 0)
      glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(GL_TRIANGLES, 0, _num_vertices);
    glBindVertexArray(0);
  }

  void Mesh::CreateCube(Mesh **mesh) {
    std::vector<f32> cube_mesh = {
      // positions          // normals           // tangents        // texcoords
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
       1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
      -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,

      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
      -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,

      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,

       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,

      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
       1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
       1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
      -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
       1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
      -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f
    };

    *mesh = new Mesh(cube_mesh);
  }

  void Mesh::CreateQuad(Mesh **mesh) {
    std::vector<f32> quad_mesh = {
      // positions         // normals         // tangents        // texcoords
      -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,

       1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f
    };

    *mesh = new Mesh(quad_mesh);
  }

  void Mesh::CreateTriangle(Mesh **mesh) {
    std::vector<f32> triangle_mesh = {
      // positions         // normals         // tangents        // texcoords
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.5f, 1.0f
    };
    *mesh = new Mesh(triangle_mesh);
  }

} // namespace axl
