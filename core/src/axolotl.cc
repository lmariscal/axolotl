#include <algorithm>
#include <axolotl/axolotl.hh>
#include <axolotl/shader.hh>
#include <ergo/path.hh>

namespace axl {

  void Axolotl::Init() {
#ifdef AXOLOTL_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    spdlog::set_pattern("[%R:%S:%e] [%^%l%$]: %v");
  }

  std::string Axolotl::GetDistDir() {
    std::string result = ergo::get_binary_path();
    std::string::size_type pos = std::string(result).rfind("dist/");
    result = result.substr(0, pos + 5);
    return result;
  }

} // namespace axl

#ifdef AXOLOTL_RUNTIME

using namespace axl;

i32 main() {
  log::error("TODO Runtime");
}

#endif
