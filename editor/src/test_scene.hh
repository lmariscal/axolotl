#pragma once

#include <axolotl/scene.hh>
#include <axolotl/types.hh>

namespace axl {

  class Camera;

  class TestScene: public Scene {
   public:
    virtual ~TestScene() = default;
    void Init(Window &window) override;
    void Update(Window &window, f64 delta) override;
    void Focused(Window &window, bool state) override;
  };

} // namespace axl
