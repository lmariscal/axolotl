#pragma once

#include <axolotl/types.hh>

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
    void SetMaterialID(u32 id);
    u32 GetMaterialID() const;

    static void CreateQuad(Mesh **mesh);
    static void CreateTriangle(Mesh **mesh);
    static void CreateCube(Mesh **mesh);

    bool two_sided;

   protected:
    friend class Model;

    u32 _vao;
    u32 _num_vertices;
    u32 _num_indices;
    u32 _material_id;
    bool _single_mesh;
    std::vector<BufferData> _buffers;

    void LoadBuffers(const std::vector<f32> &vertices, const std::vector<u32> &indices);
  };

} // namespace axl
