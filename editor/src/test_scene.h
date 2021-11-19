#pragma once

#include <axolotl/types.h>
#include <axolotl/scene.h>

namespace axl {

  class TestScene : public Scene {
   public:
    void Init() override;
    void Update(f32 delta) override;
  };

} // namespace axl
