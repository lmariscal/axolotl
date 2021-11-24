#include <axolotl/texture.h>

#include <stb_image.h>
#include <glad.h>

namespace axl {

  Texture::Texture(const std::filesystem::path &path) {
    TextureStore::RegisterTexture(*this, path);
  }

  void Texture::Init() {
    TextureStore::ProcessQueue();
  }

  json Texture::Serialize() const {
    json j = GetRootNode("texture");
    j["path"] = TextureStore::GetPath(texture_id);
    return j;
  }

  void Texture::Deserialize(const json &j) {
    if (!VerifyRootNode(j, "texture"))
      return;
  }

  bool Texture::ShowData() {
    bool modified = false;
    return modified;
  }

  void Texture::Bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureStore::GetRendererTextureID(texture_id));
  }

  u32 TextureStore::GetTextureID(const std::filesystem::path &path) {
    if (_path_to_id.count(path))
      return _path_to_id[path];
    return 0;
  }

  u32 TextureStore::GetRendererTextureID(u32 id) {
    if (_textures.count(id))
      return _textures[id];
    return 0;
  }

  void TextureStore::RegisterTexture(Texture &texture, const std::filesystem::path &path) {
    if (_path_to_id.count(path)) {
      texture.texture_id = _path_to_id[path];
      _instances[texture.texture_id]++;
      return;
    }

    _id_count++;
    texture.texture_id = _id_count;
    _path_to_id.insert(std::pair<std::filesystem::path, u32>(path, texture.texture_id));
    _textures.insert(std::pair<u32, u32>(_id_count, 0));
    _instances.insert(std::pair<u32, u32>(texture.texture_id, 1));
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
      LoadTexture(t, path);
      _texture_queue.pop_back();
    }
  }

  void TextureStore::LoadTexture(const Texture &texture, const std::filesystem::path &path) {
    log::debug("Loading Texture \"{}\"", path.string());

    // Load opengl Texture with stb_image
    i32 width, height, channels;
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

    _textures[texture.texture_id] = tex;

    stbi_image_free(data);
  }

  void TextureStore::DeregisterTexture(u32 id) {
    if (!_textures.count(id))
      return;

    _instances[id]--;
    if (_instances[id] > 0)
      return;

    glDeleteTextures(1, &_textures[id]);

    _textures.erase(id);
    _instances.erase(id);
    _path_to_id.erase(GetPath(id));
  }

} // namespace axl
