#pragma once

#include <axolotl/types.h>

#include <vector>

namespace axl {

  struct Mesh {
   public:
    Mesh(const std::vector<f32> &vertices);
    ~Mesh();

    void Draw();

   protected:
    u32 _vao;
    u32 _vbo;
    u32 _ibo;
    u32 _num_vertices;
  };

} // namespace axl
