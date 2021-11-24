#pragma once

#include <axolotl/types.h>

#include <vector>
#include <memory>
#include <map>

namespace axl {

  struct BufferData {
    u32 vbo;
    i32 target;
  };

  struct MeshData {
    MeshData();

    u32 vao;
    u32 num_vertices;
    u32 num_indices;
    std::vector<BufferData> buffers;
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

    void LoadBuffers(const std::vector<f32> &vertices, const std::vector<u32> &indices);
  };

} // namespace axl
