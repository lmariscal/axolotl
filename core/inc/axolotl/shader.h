#pragma once

#include <axolotl/types.h>
#include <axolotl/component.h>

#include <filesystem>
#include <unordered_map>
#include <vector>

namespace efsw {

  class FileWatcher;
  typedef long WatchID;

} // namespace efsw

namespace axl {

  constexpr static u32 MaxTextures = 32;

  class ShaderWatcher;
  class Axolotl;
  class Terminal;

  enum class UniformType {
    Model,
    View,
    Projection,
    Time,
    Resolution,
    Mouse,
    MouseDelta,
    Texture,
    Other,
    Last
  };

  enum class UniformDataType {
    Float,
    Int,
    Vec2,
    Vec3,
    Vec4,
    Mat4,
    Sampler2D,
    Color,
    Last
  };

  enum class ShaderType {
    Vertex,
    Fragment,
    Geometry,
    Compute,
    Last
  };

  struct Uniform {
    UniformType type;
    UniformDataType data_type;
    ShaderType shader_type;
    std::string name;
    json value;

    Uniform();
    Uniform(UniformType type, UniformDataType data_type, std::string name, ShaderType shader_type, json value);

    std::string UniformTypeToString(UniformType type);
  };

  struct ShaderPaths {
    std::filesystem::path vertex   = "";
    std::filesystem::path fragment = "";
    std::filesystem::path geometry = "";
    std::filesystem::path compute  = "";
  };

  struct Shader {
   public:
    Shader();
    Shader(const ShaderPaths &paths);
    Shader(Shader &other) = delete;
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
    void SetUniformI32(u32 location, const i32 &value);
    void SetUniformU32(const std::string &name, const u32 &value);
    void SetUniformF32V(const std::string &name, const f32 *value, u32 count);
    void SetUniformI32V(const std::string &name, const i32 *value, u32 count);
    void SetUniformU32V(const std::string &name, const u32 *value, u32 count);
    void SetUniformV2V(const std::string &name, const v2 *value, u32 count);
    void SetUniformV3V(const std::string &name, const v3 *value, u32 count);
    void SetUniformV4V(const std::string &name, const v4 *value, u32 count);
    void SetUniformM3V(const std::string &name, const m3 *value, u32 count);
    void SetUniformM4V(const std::string &name, const m4 *value, u32 count);

    void SetUniformModel(const m4 &model);
    void SetUniformView(const m4 &view);
    void SetUniformProjection(const m4 &projection);
    void SetUniformTime(const f32 &time);
    void SetUniformResolution(const v2 &resolution);
    void SetUniformMouse(const v2 &mouse);
    void SetUniformMouseDelta(const v2 &mouse_delta);
    void SetUniformTexture(u32 unit, u32 opengl_id);
    void SetOthers();

    static std::string ShaderTypeToString(ShaderType type);
    static ShaderType StringToShaderType(const std::string &str);

    void Init();
    bool ShowData();

   protected:
    friend class Material;

    friend class ShaderWatcher;
    friend class Axolotl;
    friend class Terminal;

    inline static std::vector<Shader *> _shaders_programs;

    std::unordered_map<std::string, i32> _uniform_locations;

    void ShowData(Uniform &u);
    void GetUniformData();

    void ParseUniform(std::string line, ShaderType shader_type);
    std::string Read(const std::filesystem::path &path, ShaderType shade_type);
    u32 CompileShader(ShaderType type, const std::string &data);

    u32 _program;
    u32 _shaders[(i32)ShaderType::Last];
    std::filesystem::path _paths[(i32)ShaderType::Last];
    std::vector<Uniform> _uniforms[(i32)UniformType::Last];
    u32 _uniform_texture_cache[32];

    bool _need_reload[(i32)ShaderType::Last];
    efsw::FileWatcher *_watcher;
    ShaderWatcher *_shader_watcher;
    efsw::WatchID _watch_ids[(i32)ShaderType::Last];
  };

} // namespace axl
