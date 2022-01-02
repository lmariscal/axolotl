#pragma once

#include <axolotl/line.hh>
#include <axolotl/types.hh>

namespace axl {

  class Shader;
  class LinePrimitive;

  class Grid {
   public:
    Grid(const v2i &size, const v2i &cell_size);

    void Draw(const m4 &view, const m4 &projection);

   protected:
    v2i _size;
    v2i _cell_size;

    std::unique_ptr<Shader> _shader;
    std::unique_ptr<LinePrimitive> _line;
  };

} // namespace axl
