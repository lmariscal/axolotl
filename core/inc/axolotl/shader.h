#pragma once

#include <axolotl/types.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace efsw {

  class FileWatcher;
  typedef long WatchID;

} // namespace efsw

namespace axl {

  class ShaderWatcher;
  class Axolotl;
  class Terminal;

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
    Shader(const std::filesystem::path &vertex, const std::filesystem::path &fragment, const std::filesystem::path &geometry = "", const std::filesystem::path &compute = "");
    ~Shader();

    void Bind();
    void Unload(ShaderType type);
    void Load(ShaderType type, const std::filesystem::path &path);
    void LoadData(ShaderType type, const std::string &data);
    bool Watch();
    bool Reload(ShaderType type = ShaderType::Last);
    bool Compile();
    bool Recompile();
    i32 GetUniformLocation(const std::string &name);
    void SetUniformV2(const std::string &name, const v2 &value);
    void SetUniformV3(const std::string &name, const v3 &value);
    void SetUniformV4(const std::string &name, const v4 &value);
    void SetUniformM3(const std::string &name, const m3 &value);
    void SetUniformM4(const std::string &name, const m4 &value);
    void SetUniformF32(const std::string &name, const f32 &value);
    void SetUniformI32(const std::string &name, const i32 &value);
    void SetUniformU32(const std::string &name, const u32 &value);
    void SetUniformF32V(const std::string &name, const f32 *value, u32 count);
    void SetUniformI32V(const std::string &name, const i32 *value, u32 count);
    void SetUniformU32V(const std::string &name, const u32 *value, u32 count);
    void SetUniformV2V(const std::string &name, const v2 *value, u32 count);
    void SetUniformV3V(const std::string &name, const v3 *value, u32 count);
    void SetUniformV4V(const std::string &name, const v4 *value, u32 count);
    void SetUniformM3V(const std::string &name, const m3 *value, u32 count);
    void SetUniformM4V(const std::string &name, const m4 *value, u32 count);

    static std::string ShaderTypeToString(ShaderType type);
    static ShaderType StringToShaderType(const std::string &str);

   protected:
    friend class ShaderWatcher;
    friend class Axolotl;
    friend class Terminal;

    // static std::unordered_map<std::filesystem::path, ShaderShader using shared_ptr to manage state of shaders
    static std::vector<Shader *> _shaders_programs;

    std::unordered_map<std::string, i32> _uniform_locations;

    void Init();
    std::string Read(const std::filesystem::path &path);
    u32 CompileShader(ShaderType type, const std::string &data);

    u32 _program;
    u32 _shaders[(i32)ShaderType::Last];
    std::filesystem::path _paths[(i32)ShaderType::Last];

    bool _need_reload[(i32)ShaderType::Last];
    efsw::FileWatcher *_watcher;
    ShaderWatcher *_shader_watcher;
    efsw::WatchID _watch_ids[(i32)ShaderType::Last];
  };

} // namespace axl
