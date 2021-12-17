#pragma once

#include <axolotl/mesh.hh>
#include <axolotl/types.hh>

namespace axl {

  class Line {
   public:
    Line(v3 pos0, v3 pos1, Color color = Color(), f32 thickness = 1.0f, bool loop = false);
    Line(const std::vector<v3> &positions, Color color = Color(), f32 thickness = 1.0f, bool loop = false);
    Line(const std::vector<v3> &vertices, const std::vector<v2u> &indices, Color color = Color(), f32 thickness = 1.0f);

    Line(Line &&other);
    Line(const Line &other);

    Line &operator=(Line &&other);
    Line &operator=(const Line &other);

    ~Line();

    void Draw() const;

    Color color;
    f32 thickness;
    bool loop;

   protected:
    void LoadBuffers();

    std::vector<v3> _vertices;
    std::vector<v2u> _indices;

    u32 _vao;
    u32 _vbo;
    u32 _ebo;

    inline static v2 _line_thickness_range = v2(0.0f);
  };

} // namespace axl
