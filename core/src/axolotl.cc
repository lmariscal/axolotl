#include <axolotl/axolotl.h>

namespace axl {

  void Axolotl::Init() {
#ifdef AXOLOTL_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    spdlog::set_pattern("[%R:%S:%e] [%^%l%$]: %v");
  }

} // namespace axl
