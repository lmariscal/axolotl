#pragma once

#include <axolotl/types.h>

#include <filesystem>

namespace axl {

  enum class ShaderType {
    Vertex,
    Fragment,
    Geometry,
    Compute,
    Last
  };

  struct Shader {
   public:
    Shader();
    ~Shader();

    void Load(ShaderType type, const std::filesystem::path &path);
    void LoadData(ShaderType type, const std::string &data);
    bool Reload(ShaderType type = ShaderType::Last);
    bool Compile();
    bool Recompile();

   protected:
    std::string Read(const std::filesystem::path &path);
    u32 CompileShader(ShaderType type, const std::string &data);

    u32 _program;
    u32 _shaders[(i32)ShaderType::Last];
    std::filesystem::path _paths[(i32)ShaderType::Last];
  };

} // namespace axl
