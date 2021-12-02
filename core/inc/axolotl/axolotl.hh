#pragma once

#include <axolotl/types.hh>

namespace axl {

  class Shader;

  class Axolotl {
   public:
    static void Init();
    static std::vector<Shader *> WatchShaders();
    static std::string GetDistDir();
  };

} // namespace axl
