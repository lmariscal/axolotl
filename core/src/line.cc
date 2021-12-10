#include <axolotl/line.hh>

namespace axl {

  Line::Line(v3 pos0, v3 pos1, Color color, f32 thickness):
    positions({ pos0, pos1 }),
    color(color),
    thickness(thickness) { }

  Line::Line(const std::array<v3, 2> &positions, Color color, f32 thickness):
    positions(positions),
    color(color),
    thickness(thickness) { }

  Line::~Line() { }

  void Line::Draw() { }

} // namespace axl
