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

  struct Mesh {
   public:
    Mesh(const std::vector<f32> &vertices, const std::vector<u32> &indices = { });
    ~Mesh();

    void Draw();

    static Mesh CreateQuad();
    static Mesh CreateTriangle();
    static Mesh CreateCube();

   protected:
    u32 _vao;
    u32 _num_vertices;
    u32 _num_indices;
    std::vector<BufferData> _buffers;

    void LoadBuffers(const std::vector<f32> &vertices, const std::vector<u32> &indices);
  };

} // namespace axl
