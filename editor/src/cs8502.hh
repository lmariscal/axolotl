#pragma once

#include <axolotl/types.hh>
#include <axolotl/scene.hh>

namespace axl {

  class Camera;

  class CS8502 : public Scene {
   public:
    void Init(Window &window) override;
    void Update(Window &window, f64 delta) override;
    void Focused(Window &window, bool state) override;
  };

} // namespace axl
