#pragma once

#include <axolotl/types.hh>
#include <axolotl/scene.hh>

namespace axl {

  class Camera;

  class CS8502 : public Scene {
   public:
    void Init() override;
    void Update(Window &window) override;
    void Focused(Window &window, bool state) override;
  };

} // namespace axl
