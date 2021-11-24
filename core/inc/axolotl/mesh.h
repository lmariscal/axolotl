#pragma once

#include <axolotl/types.h>

#include <vector>
#include <memory>

namespace axl {

  struct MeshData {
    MeshData();

    u32 vao;
    u32 vbo;
    u32 ibo;
    u32 num_vertices;
    u32 num_indices;
  };

  struct Mesh {
   public:
    Mesh(const std::vector<f32> &vertices, const std::vector<u32> &indices = { });
    ~Mesh();

    void Draw();

    static Mesh CreateQuad();
    static Mesh CreateTriangle();
    static Mesh CreateCube();

   protected:
    std::shared_ptr<MeshData> _data;
  };

} // namespace axl
