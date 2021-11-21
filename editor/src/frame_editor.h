#include <axolotl/framebuffer.h>

#include <axolotl/types.h>
#include <axolotl/renderer.h>

namespace axl {

  class FrameEditor {
   public:
    FrameEditor();
    ~FrameEditor();

    void Bind(Window &window);
    void Unbind(Window &window);
    void Draw(Window &window);
    void SetBoundFrameRatio(bool state);

    bool bound_frame_ratio;

   protected:
    FrameBuffer _frame;
    v2 _region_available;
  };

} // namespace axl
