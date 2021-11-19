#include <axolotl/axolotl.h>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <algorithm>
#include <axolotl/shader.h>

namespace axl {

  void Axolotl::Init() {
#ifdef AXOLOTL_DEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    spdlog::set_pattern("[%R:%S:%e] [%^%l%$]: %v");
  }

  std::string Axolotl::GetDistDir() {
    char *result = new char[512];
    std::fill(result, result + 512, '\0');

#if defined(WIN32) || defined(_WIN32)
    GetModuleFileName(nullptr, resultChar, MAX_PATH);
#elif defined(__linux__)
    ssize_t count = readlink("/proc/self/exe", result, 512);
#else
    std::cerr << "Unsupported OS for DistPath\n";
#endif

    std::string resultStr = std::string(result);
    delete[] result;

    std::replace(resultStr.begin(), resultStr.end(), '\\', '/');
    std::string::size_type pos = std::string(resultStr).rfind("dist/");
    resultStr = resultStr.substr(0, pos + 5);

    return resultStr;
  }

  std::vector<Shader *> Axolotl::WatchShaders() {
    std::vector<Shader *> need_recompile;
    for (Shader *shader : Shader::_shaders_programs) {
      if (!shader->Watch())
        continue;
      need_recompile.push_back(shader);
    }
    return need_recompile;
  }

} // namespace axl
