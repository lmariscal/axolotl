#include <axolotl/mesh.h>

#include <glad.h>

namespace axl {

  MeshData::MeshData():
    vao(0),
    num_vertices(0),
    num_indices(0)
  {  }

  Mesh::Mesh(const std::vector<f32> &vertices, const std::vector<u32> &indices):
    _data(new MeshData())
  {
    // vec3 positions
    // vec3 normals
    // vec2 texcoords
    _data->num_vertices = vertices.size() / 8;
    _data->num_indices = indices.size();

    LoadBuffers(vertices, indices);
  }

  void Mesh::LoadBuffers(const std::vector<f32> &vertices, const std::vector<u32> &indices) {
    log::debug("Creating mesh with {} vertices and {} indices", _data->num_vertices, _data->num_indices);
    glGenVertexArrays(1, &_data->vao);
    glBindVertexArray(_data->vao);

    u32 vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);
    _data->buffers.push_back({ vbo, GL_ARRAY_BUFFER });

    if (indices.size() > 0) {
      u32 ibo = 0;
      glGenBuffers(1, &ibo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
      _data->buffers.push_back({ ibo, GL_ELEMENT_ARRAY_BUFFER });
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6 * sizeof(f32)));

    glBindVertexArray(0);
  }

  Mesh::~Mesh() {
    if (_data.use_count() > 1)
      return;
    log::debug("Deleting mesh {}", _data->vao);
    for (auto &buffer : _data->buffers)
      glDeleteBuffers(1, &buffer.vbo);
    glDeleteVertexArrays(1, &_data->vao);
  }

  void Mesh::Draw() {
    glBindVertexArray(_data->vao);
    if (_data->num_indices > 0)
      glDrawElements(GL_TRIANGLES, _data->num_indices, GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(GL_TRIANGLES, 0, _data->num_vertices);
    glBindVertexArray(0);
  }

  Mesh Mesh::CreateCube() {
    std::vector<f32> cube_mesh = {
      // positions          // normals           // texcoords
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
       1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
      -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
       1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
       1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
       1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
       1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
      -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
       1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
      -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    return Mesh(cube_mesh);
  }

  Mesh Mesh::CreateQuad() {
    std::vector<f32> quad_mesh = {
      // positions         // normals           // texcoords
      -1.0f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,

       1.0f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f
    };

    return Mesh(quad_mesh);
  }

  Mesh Mesh::CreateTriangle() {
    std::vector<f32> triangle_mesh = {
      // positions         // normals           // texcoords
      -1.0f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
       0.0f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  0.5f, 1.0f
    };
    return Mesh(triangle_mesh);
  }

} // namespace axl
