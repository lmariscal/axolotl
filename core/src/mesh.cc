#include <axolotl/mesh.h>

#include <glad.h>

namespace axl {

  MeshData::MeshData():
    vao(0),
    vbo(0),
    ibo(0),
    num_vertices(0),
    num_indices(0)
  {  }

  Mesh::Mesh(const std::vector<f32> &vertices, const std::vector<u32> &indices):
    _data(new MeshData())
  {
    _data->num_vertices = vertices.size() / 5;
    _data->num_indices = indices.size();

    log::debug("Creating mesh with {} vertices and {} indices", _data->num_vertices, _data->num_indices);
    glGenVertexArrays(1, &_data->vao);
    glBindVertexArray(_data->vao);

    glGenBuffers(1, &_data->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _data->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(f32), vertices.data(), GL_STATIC_DRAW);

    if (indices.size() > 0) {
      glGenBuffers(1, &_data->ibo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _data->ibo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
    }

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3 * sizeof(f32)));

    glBindVertexArray(0);
  }

  Mesh::~Mesh() {
    if (_data.use_count() > 1)
      return;
    log::debug("Destroying mesh vao {}, vbo {}, ibo {}", _data->vao, _data->vbo, _data->ibo);
    glDeleteBuffers(1, &_data->vbo);
    glDeleteBuffers(1, &_data->ibo);
    glDeleteVertexArrays(1, &_data->vao);
    if (_data->ibo)
      glDeleteBuffers(1, &_data->ibo);
  }

  void Mesh::Draw() {
    glBindVertexArray(_data->vao);
    if (_data->ibo)
      glDrawElements(GL_TRIANGLES, _data->num_indices, GL_UNSIGNED_INT, 0);
    else
      glDrawArrays(GL_TRIANGLES, 0, _data->num_vertices);
    glBindVertexArray(0);
  }

  Mesh Mesh::CreateQuad() {
    std::vector<f32> triangle_mesh = {
       1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
       1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,  0.0f, 1.0f
    };
    std::vector<u32> triangle_indices = {
      0, 1, 2,
      0, 2, 3,
    };

    return Mesh(triangle_mesh, triangle_indices);
  }

  Mesh Mesh::CreateTriangle() {
    std::vector<f32> triangle_mesh = {
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
       0.0f,  1.0f, 0.0f,  0.5f, 1.0f
    };
    return Mesh(triangle_mesh);
  }

  Mesh Mesh::CreateCube() {
    std::vector<f32> cube_mesh = {
      // front
      -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
      -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
      // back
      -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
       1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
       1.0f,  1.0f, -1.0f,  0.0f, 1.0f,
      -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,
      // left
      -1.0f, -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,  1.0f, 1.0f,
      -1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,
      // right
       1.0f, -1.0f, -1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f,  1.0f, 1.0f,
       1.0f,  1.0f,  1.0f,  1.0f, 0.0f,
       1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
      // top
      -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,
       1.0f,  1.0f,  1.0f,  1.0f, 1.0f,
       1.0f,  1.0f, -1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, -1.0f,  0.0f, 0.0f,
      // bottom
      -1.0f, -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f,  1.0f, 1.0f,
       1.0f, -1.0f, -1.0f,  1.0f, 0.0f,
      -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
    };

    std::vector<u32> cube_indices = {
      0,  1,  2,  0,  2,  3,
      4,  5,  6,  4,  6,  7,
      8,  9,  10,  8,  10, 11,
      12, 13, 14, 12, 14, 15,
      16, 17, 18, 16, 18, 19,
      20, 21, 22, 20, 22, 23
    };

    return Mesh(cube_mesh, cube_indices);
  }

} // namespace axl
