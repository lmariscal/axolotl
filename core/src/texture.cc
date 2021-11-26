#include <axolotl/texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad.h>

namespace axl {

  Texture::Texture(const std::filesystem::path &path, TextureType type, const  TextureData &data):
    type(type)
  {
    TextureStore::RegisterTexture(*this, path, type, data);
  }


  std::string Texture::TextureTypeToString(TextureType type) {
    switch (type) {
      case TextureType::Diffuse:
        return "diffuse";
      case TextureType::Specular:
        return "specular";
      case TextureType::Normal:
        return "normal";
      case TextureType::Ambient:
        return "ambient";
      case TextureType::Normals:
        return "normals";
      case TextureType::Height:
        return "height";
      default:
        return "";
    }
  }

  void Texture::Init() {
    TextureStore::ProcessQueue();
  }

  void Texture::Bind(u32 unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, TextureStore::GetRendererTextureID(texture_id));
  }

  u32 TextureStore::GetTextureID(const std::filesystem::path &path) {
    if (_path_to_id.count(path))
      return _path_to_id[path];
    return 0;
  }

  u32 TextureStore::GetRendererTextureID(u32 id) {
    if (_data.count(id))
      return _data[id].gl_id;
    return 0;
  }

  void TextureStore::RegisterTexture(Texture &texture, const std::filesystem::path &path,
                                     TextureType type, const TextureData &data)
  {
    if (!path.empty() && _path_to_id.count(path)) {
      texture.texture_id = _path_to_id[path];
      _data[texture.texture_id].instances++;
      return;
    }

    _id_count++;
    texture.texture_id = _id_count;
    texture.type = type;
    if (!path.empty())
      _path_to_id.insert(std::pair<std::filesystem::path, u32>(path, texture.texture_id));
    _data.insert(std::pair<u32, TextureData>(_id_count, data));
    _texture_queue.push_back(texture);
    return;
  }

  std::filesystem::path TextureStore::GetPath(u32 id) {
    for (auto it = _path_to_id.cbegin(); it != _path_to_id.cend(); ++it) {
      if (it->second == id)
        return it->first;
    }
    return "";
  }

  void TextureStore::ProcessQueue() {
    while (!_texture_queue.empty()) {
      Texture t = *(--_texture_queue.end());
      std::filesystem::path path = GetPath(t.texture_id);
      if (!path.empty())
        LoadTexture(t, path);
      else
        CreateTexture(t);
      _texture_queue.pop_back();
    }
  }

  void TextureStore::LoadTexture(const Texture &texture, const std::filesystem::path &path) {
    log::debug("Loading Texture \"{}\", type {}", path.string(), Texture::TextureTypeToString(texture.type));

    // Load opengl Texture with stb_image
    i32 width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    u8 *data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data) {
      log::error("Failed to load texture \"{}\"", path.string());
      return;
    }

    u32 tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    _data[texture.texture_id].gl_id = tex;
    _data[texture.texture_id].size = v2i(width, height);

    stbi_image_free(data);
  }

  void TextureStore::CreateTexture(const Texture &texture) {
    v2i &size = _data[texture.texture_id].size;
    if (size.x <= 0 || size.y <= 0) {
      DeregisterTexture(texture.texture_id);
      return;
    }
  }

  void TextureStore::DeregisterTexture(u32 id) {
    if (!_data.count(id))
      return;

    _data[id].instances--;
    if (_data[id].instances > 0)
      return;

    log::debug("Deleting Texture \"{}\"", GetPath(id).string());

    if (_data[id].gl_id)
      glDeleteTextures(1, &_data[id].instances);

    _data.erase(id);
    _path_to_id.erase(GetPath(id));
  }

} // namespace axl
