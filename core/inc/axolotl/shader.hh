#pragma once

#include <axolotl/types.hh>
#include <axolotl/component.hh>
#include <axolotl/texture.hh>

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <array>

namespace efsw {

  class FileWatcher;
  typedef long WatchID;

} // namespace efsw

namespace axl {

  constexpr static u32 MaxTextures = 32;

  class ShaderWatcher;
  class Axolotl;
  class Terminal;

  enum class UniformLocation {
    // Vertex
    ModelMatrix      = 0,
    ViewMatrix       = 1,
    ProjectionMatrix = 2,
    Time             = 3,
    Resolution       = 4,
    Mouse            = 5,

    // Fragment
    Textures         = 10,
    Last
  };

  enum class AttributeLocation {
    Position = 0,
    Normal   = 1,
    TexCoord = 2,
    Last
  };

  enum class UniformDataType {
    Vector2,
    Vector3,
    Vector4,
    Matrix3,
    Matrix4,
    Float,
    Double,
    Int,
    UInt,
    Texture,
    TextureArray,
    Last
  };

  enum class ShaderType {
    Vertex,
    Fragment,
    Geometry,
    Compute,
    Last
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
    Shader(Shader &&other) = delete;
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
    UniformDataType GLToUniformDataType(u32 type);
    u32 UniformDataTypeToGL(UniformDataType type);

#pragma region Uniforms
    void SetUniformV2(u32 location, const v2 &value);
    void SetUniformV3(u32 location, const v3 &value);
    void SetUniformV4(u32 location, const v4 &value);
    void SetUniformM3(u32 location, const m3 &value);
    void SetUniformM4(u32 location, const m4 &value);
    void SetUniformF32(u32 location, const f32 &value);
    void SetUniformI32(u32 location, const i32 &value);
    void SetUniformU32(u32 location, const u32 &value);
    void SetUniformTexture(TextureType type, i32 gl_id);

    void SetUniformV2(const std::string &name, const v2 &value);
    void SetUniformV3(const std::string &name, const v3 &value);
    void SetUniformV4(const std::string &name, const v4 &value);
    void SetUniformM3(const std::string &name, const m3 &value);
    void SetUniformM4(const std::string &name, const m4 &value);
    void SetUniformF32(const std::string &name, const f32 &value);
    void SetUniformI32(const std::string &name, const i32 &value);
    void SetUniformU32(const std::string &name, const u32 &value);
#pragma endregion

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

    void GetUniformData();
    void VerifyUniforms();
    std::string Read(const std::filesystem::path &path, ShaderType shade_type);
    u32 CompileShader(ShaderType type, const std::string &data);

    std::unordered_map<i32, UniformDataType> _uniform_data_type;
    std::unordered_map<std::string, i32> _uniform_locations;
    std::unordered_map<i32, std::string> _uniform_locations_reverse;

    // uniform cache
    std::unordered_map<i32, v2> _uniform_v2;
    std::unordered_map<i32, v3> _uniform_v3;
    std::unordered_map<i32, v4> _uniform_v4;
    std::unordered_map<i32, m3> _uniform_m3;
    std::unordered_map<i32, m4> _uniform_m4;
    std::unordered_map<i32, f32> _uniform_f32;
    std::unordered_map<i32, f64> _uniform_f64;
    std::unordered_map<i32, i32> _uniform_i32;
    std::unordered_map<i32, u32> _uniform_u32;
    std::unordered_map<i32, std::array<i32, MaxTextures>> _uniform_textures;
    // uniform cache

    u32 _program;
    u32 _shaders[(i32)ShaderType::Last];
    std::filesystem::path _paths[(i32)ShaderType::Last];

    bool _need_reload[(i32)ShaderType::Last];
    efsw::FileWatcher *_watcher;
    ShaderWatcher *_shader_watcher;
    efsw::WatchID _watch_ids[(i32)ShaderType::Last];
  };

} // namespace axl
