#pragma once

#include <axolotl/types.hh>

#include <unordered_map>
#include <queue>

namespace axl {

  constexpr u32 MAX_TEXTURE_UNITS = 32;

  enum class TextureType {
    Diffuse,
    Specular,
    Normal,
    Ambient,
    Height,
    Buffer,
    Last
  };

  enum class TextureInternalFormat {
    RGB,
    RGBA,
    Depth,
    DepthStencil,
    Last
  };

  enum class TextureFormat {
    RGB,
    RGBA,
    Depth,
    Stencil,
    DepthStencil,
    Last
  };

  enum class TextureDataType {
    U8,
    U16,
    U24_8,
    U32,
    I8,
    I16,
    I32,
    F16,
    F32,
    Last
  };

  struct TextureData {
    u32 instances = 0;
    u32 gl_id = 0;
    v2i size = v2i(0);
    bool loaded = false;

    TextureFormat format = TextureFormat::Last;
    TextureInternalFormat internal_format = TextureInternalFormat::Last;
    TextureDataType data_type = TextureDataType::Last;
  };

  struct Texture {
    Texture(const std::filesystem::path &path = "", TextureType type = TextureType::Last, const TextureData &data = { });
    Texture(const Texture &other);
    Texture(Texture &&other);
    ~Texture();

    void Init();
    void Bind(u32 unit = 0);

    u32 texture_id = 0;
    TextureType type = TextureType::Last;

    operator u32() const;

    static std::string TextureTypeToString(TextureType type);
  };

  class TextureStore {
   public:
    static u32 GetTextureID(const std::filesystem::path &path);
    static std::filesystem::path GetPath(u32 id);
    static u32 GetRendererID(u32 id);
    static TextureData & GetData(u32);
    static void RegisterTexture(Texture &texture, const std::filesystem::path &path, TextureType type, const TextureData &data);
    static void ProcessQueue();
    static void DeregisterTexture(u32 id);

   protected:
    inline static u32 _id_counter = 0;
    inline static std::unordered_map<u32, TextureData> _data; // renderer_id -> opengl_id
    inline static std::map<std::filesystem::path, u32> _path_to_id; // path -> renderer_id
    inline static std::queue<Texture> _texture_queue;

    static void LoadTexture(const Texture &texture, const std::filesystem::path &path);
    static void CreateTexture(const Texture &texture);
  };

} // namespace axl