#pragma once

#include <axolotl/types.hh>

namespace axl {

  class Grid {
   public:
    static void Draw(const m4 &view, const m4 &proj, i32 size);
  };

} // namespace axl
