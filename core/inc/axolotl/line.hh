#pragma once

#include <axolotl/mesh.hh>
#include <axolotl/types.hh>

namespace axl {

  class Line {
   public:
    Line(v3 pos0, v3 pos1, Color color = Color(), f32 thickness = 1.0f);
    Line(const std::array<v3, 2> &positions, Color color = Color(), f32 thickness = 1.0f);
    ~Line();

    void Draw();

    Color color;
    std::array<v3, 2> positions;
    f32 thickness;
  };

} // namespace axl
