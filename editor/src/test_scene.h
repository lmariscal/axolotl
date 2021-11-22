#pragma once

#include <axolotl/types.h>
#include <axolotl/scene.h>

namespace axl {

  class Camera;

  class TestScene : public Scene {
   public:
    void Init() override;
    void Update(Window &window) override;
  };

} // namespace axl
