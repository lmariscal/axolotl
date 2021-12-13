#include <algorithm>
#include <axolotl/axolotl.hh>
#include <axolotl/component.hh>
#include <axolotl/shader.hh>
#include <ergo/path.hh>

namespace axl {

  void Axolotl::Init() {
    using namespace entt::literals;

#ifdef AXOLOTL_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    spdlog::set_pattern("[%R:%S:%e] [%^%l%$]: %v");

    // Register component member data types
    REGISTER_COMPONENT_DATA_TYPE(f32);
    REGISTER_COMPONENT_DATA_TYPE(u32);
    REGISTER_COMPONENT_DATA_TYPE(bool);
    REGISTER_COMPONENT_DATA_TYPE(std::string);
    REGISTER_COMPONENT_DATA_TYPE(std::filesystem::path);

    REGISTER_COMPONENT_DATA_TYPE(std::vector<uuid>);
    REGISTER_COMPONENT_DATA_TYPE(std::vector<std::string>);

    REGISTER_COMPONENT_DATA_TYPE(v2);
    REGISTER_COMPONENT_DATA_TYPE(v3);
    REGISTER_COMPONENT_DATA_TYPE(v4);
    REGISTER_COMPONENT_DATA_TYPE(quat);

    REGISTER_COMPONENT_DATA_TYPE(Color);
    REGISTER_COMPONENT_DATA_TYPE(uuid);
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
