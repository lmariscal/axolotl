#pragma once

#include <axolotl/mesh.hh>
#include <axolotl/types.hh>

namespace axl {

  // RGBA has to be normalized from 255
  struct LineVertex {
    v3 pos;
    v4 color;
  };

  class LinePrimitive {
   public:
    LinePrimitive(const v3 &pos0,
                  const v3 &pos1,
                  const Color &color = Color(),
                  f32 thickness = 1.0f,
                  bool loop = false);
    LinePrimitive(const std::vector<LineVertex> &vertices, f32 thickness = 1.0f, bool loop = false);
    LinePrimitive(const std::vector<LineVertex> &vertices, const std::vector<v2u> &indices, f32 thickness = 1.0f);

    LinePrimitive(LinePrimitive &&other);
    LinePrimitive(const LinePrimitive &other);

    LinePrimitive &operator=(LinePrimitive &&other);
    LinePrimitive &operator=(const LinePrimitive &other);

    ~LinePrimitive();

    void CreateBuffers();
    void Draw() const;
    void SetColor(const Color &color);
    void SetPos(const v3 &pos0, const v3 &pos1, const Color &color = Color());

    f32 thickness;
    bool loop;

   protected:
    void LoadBuffers();

    std::vector<LineVertex> _vertices;
    std::vector<v2u> _indices;
    v4 _color;

    u32 _vao;
    u32 _vbo;
    u32 _ebo;

    inline static v2 _line_thickness_range = v2(0.0f);
  };

} // namespace axl
