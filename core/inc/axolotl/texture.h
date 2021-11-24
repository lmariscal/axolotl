#pragma once

#include <axolotl/types.h>
#include <map>

namespace axl {

  struct Texture {
    Texture(const std::filesystem::path &path = "");

    void Init();
    void Bind();

    u32 texture_id;
  };

  class TextureStore {
   public:
    static u32 GetTextureID(const std::filesystem::path &path);
    static std::filesystem::path GetPath(u32 id);
    static u32 GetRendererTextureID(u32 id);
    static void RegisterTexture(Texture &texture, const std::filesystem::path &path);
    static void ProcessQueue();
    static void DeregisterTexture(u32 id);

   protected:
    inline static u32 _id_count = 0;
    inline static std::map<u32, u32> _instances;
    inline static std::map<u32, u32> _textures;
    inline static std::map<std::filesystem::path, u32> _path_to_id;
    inline static std::vector<Texture> _texture_queue;

    static void LoadTexture(const Texture &texture, const std::filesystem::path &path);
  };

} // namespace axl
