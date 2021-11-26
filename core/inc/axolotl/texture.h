#pragma once

#include <axolotl/types.h>
#include <map>

namespace axl {

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
    Stencil,
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
    u8,
    u16,
    U32,
    U64,
    I8,
    I16,
    I32,
    I64,
    Last
  };

  struct TextureData {
    u32 instances = 0;
    u32 gl_id = 0;
    v2i size = v2i(0);

    TextureFormat format = TextureFormat::Last;
    TextureInternalFormat internal_format = TextureInternalFormat::Last;
    TextureDataType data_type = TextureDataType::Last;
  };

  struct Texture {
    Texture(const std::filesystem::path &path = "", TextureType type = TextureType::Last, const TextureData &data = { });

    void Init();
    void Bind(u32 unit = 0);

    u32 texture_id = 0;
    TextureType type = TextureType::Last;

    static std::string TextureTypeToString(TextureType type);
  };

  class TextureStore {
   public:
    static u32 GetTextureID(const std::filesystem::path &path);
    static std::filesystem::path GetPath(u32 id);
    static u32 GetRendererTextureID(u32 id);
    static const TextureData & GetData(u32);
    static void RegisterTexture(Texture &texture, const std::filesystem::path &path, TextureType type, const TextureData &data);
    static void ProcessQueue();
    static void DeregisterTexture(u32 id);

   protected:
    inline static u32 _id_count = 0;
    inline static std::map<u32, TextureData> _data; // renderer_id -> opengl_id
    inline static std::map<std::filesystem::path, u32> _path_to_id; // path -> renderer_id
    inline static std::vector<Texture> _texture_queue;

    static void LoadTexture(const Texture &texture, const std::filesystem::path &path);
    static void CreateTexture(const Texture &texture);
  };

} // namespace axl
