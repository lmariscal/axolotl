#pragma once

#include <axolotl/types.h>

namespace axl {

  class Renderer;
  class Window;

  class GUI {
   public:
    GUI(Renderer *renderer, Window *window);
    ~GUI();

    void Update();
    void Draw();

   private:
    friend class Renderer;

    Renderer *_renderer;
  };

} // namespace axl
