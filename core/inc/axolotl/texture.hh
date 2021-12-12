#pragma once

#include <axolotl/types.hh>
#include <queue>
#include <unordered_map>

namespace axl {

  constexpr u32 MAX_TEXTURE_UNITS = 32;

  enum class TextureType { Skybox, Diffuse, Specular, Normal, Ambient, Buffer, Last };

  enum class TextureInternalFormat { RGB, RGBA, Depth, DepthStencil, Last };

  enum class TextureFormat { RGB, RGBA, Depth, Stencil, DepthStencil, Last };

  enum class TextureDataType { U8, U16, U24_8, U32, I8, I16, I32, F16, F32, Last };

  class GLTexture {
   public:
    u32 id;
  };

  class TextureData {
   public:
    u32 instances = 0;
    v2i size = v2i(0);
    u32 gl_id = 0;
    bool loaded = false;

    TextureFormat format = TextureFormat::Last;
    TextureInternalFormat internal_format = TextureInternalFormat::Last;
    TextureDataType data_type = TextureDataType::Last;

   protected:
    friend class Texture2D;
    friend class TextureCube;
    friend class TextureStore;

    bool cubemap = false;
  };

  class Texture2D {
   public:
    Texture2D(const std::filesystem::path &path = "",
              TextureType type = TextureType::Last,
              const TextureData &data = {});
    Texture2D(const Texture2D &other);
    Texture2D(Texture2D &&other);
    ~Texture2D();

    void Init();
    void Bind(u32 unit = 0);

    u32 texture_id = 0;
    TextureType type = TextureType::Last;

    operator u32() const;

    static std::string TextureTypeToString(TextureType type);
  };

  class TextureCube {
   public:
    TextureCube(const std::filesystem::path &path = "",
                TextureType type = TextureType::Last,
                const TextureData &data = {});
    TextureCube(const TextureCube &other);
    TextureCube(TextureCube &&other);
    ~TextureCube();

    void Init();
    void Bind();

    u32 texture_id = 0;
    TextureType type = TextureType::Last;

    operator u32() const;
  };

  class TextureStore {
   public:
    static u32 GetTextureID(const std::filesystem::path &path);
    static std::filesystem::path GetPath(u32 id);
    static u32 GetRendererID(u32 id);
    static TextureData &GetData(u32);
    static void
    RegisterTexture(Texture2D &texture, const std::filesystem::path &path, TextureType type, const TextureData &data);
    static void
    RegisterTexture(TextureCube &texture, const std::filesystem::path &path, TextureType type, const TextureData &data);
    static void ProcessQueue();
    static void DeregisterTexture(u32 id);

   protected:
    inline static u32 _id_counter = 0;
    inline static std::unordered_map<u32, TextureData> _data;       // renderer_id -> texture_data
    inline static std::map<std::filesystem::path, u32> _path_to_id; // path -> renderer_id
    inline static std::queue<Texture2D> _texture_2d_queue;
    inline static std::queue<TextureCube> _texture_cube_queue;

    static void LoadCubemap(const TextureCube &texture, const std::filesystem::path &path);
    static void LoadTexture(const Texture2D &texture, const std::filesystem::path &path);
    static void CreateTexture(const Texture2D &texture);
  };

} // namespace axl
