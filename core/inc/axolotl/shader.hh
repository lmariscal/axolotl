#pragma once

#include <array>
#include <axolotl/component.hh>
#include <axolotl/texture.hh>
#include <axolotl/types.hh>
#include <filesystem>
#include <queue>
#include <unordered_map>
#include <vector>

namespace efsw {

  class FileWatcher;
  typedef long WatchID;

} // namespace efsw

namespace axl {

  class ShaderWatcher;

  enum class UniformLocation {
    // Vertex
    ModelMatrix = 0,
    ViewMatrix = 1,
    ProjectionMatrix = 2,
    Time = 3,
    Resolution = 4,
    Mouse = 5,

    // Fragment
    Textures = 10,
    Lights = 18,
    Last
  };

  enum class AttributeLocation { Position = 0, Normal = 1, Tangent = 2, TexCoord = 3, Last };

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

  enum class ShaderType { Vertex, Fragment, Geometry, Compute, Last };

  struct ShaderData {
    ShaderData() = default;
    ShaderData(const std::filesystem::path &vertex_path,
               const std::filesystem::path &fragment_path,
               const std::filesystem::path &geometry_path = std::filesystem::path(),
               const std::filesystem::path &compute_path = std::filesystem::path()) {
      for (i32 i = 0; i < (i32)ShaderType::Last; ++i)
        paths[i] = std::filesystem::path();

      if (!vertex_path.empty()) paths[(i32)ShaderType::Vertex] = vertex_path;
      if (!fragment_path.empty()) paths[(i32)ShaderType::Fragment] = fragment_path;
      if (!geometry_path.empty()) paths[(i32)ShaderType::Geometry] = geometry_path;
      if (!compute_path.empty()) paths[(i32)ShaderType::Compute] = compute_path;

      std::fill(shaders, shaders + (i32)ShaderType::Last, 0);
    }

    u32 id = 0;
    u32 instances = 0;
    u32 gl_id = 0;
    u32 shaders[(i32)ShaderType::Last] = { 0 };
    bool loaded = false;
    std::filesystem::path paths[(i32)ShaderType::Last] = { "" };

   protected:
    friend class ShaderStore;
    friend class Shader;

    std::unordered_map<std::string, i32> _uniform_locations;
    std::unordered_map<i32, std::string> _uniform_locations_reverse;
    std::unordered_map<std::string, i32> _attribute_locations;
    std::unordered_map<i32, std::string> _attribute_locations_reverse;
    std::unordered_map<i32, UniformDataType> _uniform_data_types;

    std::unordered_map<i32, v2> _uniform_v2;
    std::unordered_map<i32, v3> _uniform_v3;
    std::unordered_map<i32, v4> _uniform_v4;
    std::unordered_map<i32, m3> _uniform_m3;
    std::unordered_map<i32, m4> _uniform_m4;
    std::unordered_map<i32, f32> _uniform_f32;
    std::unordered_map<i32, f64> _uniform_f64;
    std::unordered_map<i32, i32> _uniform_i32;
    std::unordered_map<i32, u32> _uniform_u32;
    std::unordered_map<i32, std::array<i32, MAX_TEXTURE_UNITS>> _uniform_textures;

    efsw::FileWatcher *_file_watcher = nullptr;
    ShaderWatcher *_watcher = nullptr;
    std::array<efsw::WatchID, (i32)ShaderType::Last> _watch_ids;
  };

  class Shader {
   public:
    Shader(u32 shader_id);
    Shader(const ShaderData &data);
    Shader(const Shader &other);
    Shader(Shader &&other);
    ~Shader();

    void Bind();
    void Unbind();
    void UnloadShader(ShaderType type);
    void LoadFromPath(ShaderType type, const std::filesystem::path &path);
    void LoadFromData(ShaderType type, const std::string &data);
    bool ReloadShader(ShaderType type = ShaderType::Last);
    bool ReloadProgram();
    bool Compile();
    bool Recompile();

    i32 GetUniformLocation(const std::string &name);

    void ShowComponent();

    static UniformDataType GLToUniformDataType(u32 type);
    static u32 UniformDataTypeToGL(UniformDataType type);
    static std::string ShaderTypeToString(ShaderType type);
    static ShaderType StringToShaderType(const std::string &str);

#pragma region uniforms
    u32 GetUniformBlockIndex(const std::string &name);
    void SetUniformBlockBinding(u32 index, u32 binding);

    void SetUniformV2(u32 location, const v2 &value);
    void SetUniformV3(u32 location, const v3 &value);
    void SetUniformV3V(u32 location, const std::vector<v3> &value);
    void SetUniformV4(u32 location, const v4 &value);
    void SetUniformM3(u32 location, const m3 &value);
    void SetUniformM4(u32 location, const m4 &value);
    void SetUniformF32(u32 location, const f32 &value);
    void SetUniformI32(u32 location, const i32 &value);
    void SetUniformU32(u32 location, const u32 &value);
    void SetUniformTexture(TextureType type, i32 gl_id);

    void SetUniformV2(const std::string &name, const v2 &value);
    void SetUniformV3(const std::string &name, const v3 &value);
    void SetUniformV3V(const std::string &name, const std::vector<v3> &value);
    void SetUniformV4(const std::string &name, const v4 &value);
    void SetUniformM3(const std::string &name, const m3 &value);
    void SetUniformM4(const std::string &name, const m4 &value);
    void SetUniformF32(const std::string &name, const f32 &value);
    void SetUniformI32(const std::string &name, const i32 &value);
    void SetUniformU32(const std::string &name, const u32 &value);
#pragma endregion

    u32 shader_id = 0;

    operator u32() const;
  };

  class ShaderStore {
   public:
    static u32 GetShaderFromPath(const ShaderData &data);
    static u32 GetRendererID(u32 id);
    static ShaderData &GetData(u32 id);
    static Shader FromID(u32 id);
    static void RegisterShader(Shader &shader, const ShaderData &data);
    static void ProcessQueue();
    static void DeregisterShader(u32 id);
    static std::unordered_map<u32, ShaderData> &GetAllShadersData();

   protected:
    friend class Shader;
    friend class ShaderWatcher;

    inline static u32 _id_counter = 0;
    inline static std::unordered_map<u32, ShaderData> _shader_data;
    inline static std::queue<Shader> _shader_queue;
    inline static std::queue<std::tuple<u32, ShaderType>> _reload_queue;
    inline static Texture2D *_white_texture = nullptr;

    static u32 CompileShader(ShaderType type, const std::string &source);
    static void GetUniformData(Shader &shader);
    static void VerifyUniforms(Shader &shader);
    static void UnloadShader(Shader &shader, ShaderType type);
    static void LoadFromPath(Shader &shader, ShaderType type, const std::filesystem::path &path);
    static void LoadFromData(Shader &shader, ShaderType type, const std::string &data);
    static std::string ReadShader(ShaderType type, const std::filesystem::path &path);
    static bool ReloadShader(Shader &shader, ShaderType type = ShaderType::Last);
    static bool ReloadProgram(Shader &shader);
    static bool CompileProgram(Shader &shader);
    static bool RecompileProgram(Shader &shader);
  };

} // namespace axl
