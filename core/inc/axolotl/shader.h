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

    void Bind();
    void Unload(ShaderType type);
    void Load(ShaderType type, const std::filesystem::path &path);
    void LoadData(ShaderType type, const std::string &data);
    bool Watch();
    bool Reload(ShaderType type = ShaderType::Last);
    bool Compile();
    bool Recompile();

   protected:
    friend class ShaderWatcher;
    friend class Axolotl;

    // static std::unordered_map<std::filesystem::path, ShaderShader using shared_ptr to manage state of shaders
    static std::vector<Shader *> _shaders_programs;

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
