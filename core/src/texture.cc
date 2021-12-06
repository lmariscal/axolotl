#include <axolotl/texture.hh>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad.h>

namespace axl {

  TextureCube::TextureCube(const std::filesystem::path &path, TextureType type, const  TextureData &data):
    type(type)
  {
    TextureStore::RegisterTexture(*this, path, type, data);
  }

  TextureCube::TextureCube(const TextureCube &other) {
    TextureStore::GetData(other.texture_id).instances++;
    texture_id = other.texture_id;
    type = other.type;
  }

  TextureCube::TextureCube(TextureCube &&other) {
    TextureStore::GetData(other.texture_id).instances++;
    texture_id = other.texture_id;
    type = other.type;
  }

  TextureCube::~TextureCube() {
    TextureStore::DeregisterTexture(texture_id);
  }

  void TextureCube::Init() {
    TextureStore::ProcessQueue();
  }

  void TextureCube::Bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureStore::GetRendererID(texture_id));
  }

  Texture2D::Texture2D(const std::filesystem::path &path, TextureType type, const  TextureData &data):
    type(type)
  {
    TextureStore::RegisterTexture(*this, path, type, data);
  }

  Texture2D::Texture2D(const Texture2D &other) {
    TextureStore::GetData(other.texture_id).instances++;
    texture_id = other.texture_id;
    type = other.type;
  }

  Texture2D::Texture2D(Texture2D &&other) {
    TextureStore::GetData(other.texture_id).instances++;
    texture_id = other.texture_id;
    type = other.type;
  }

  Texture2D::~Texture2D() {
    TextureStore::DeregisterTexture(texture_id);
  }

  std::string Texture2D::TextureTypeToString(TextureType type) {
    switch (type) {
      case TextureType::Diffuse:
        return "diffuse";
      case TextureType::Specular:
        return "specular";
      case TextureType::Normal:
        return "normal";
      case TextureType::Ambient:
        return "ambient";
      default:
        return "";
    }
  }

  void Texture2D::Init() {
    TextureStore::ProcessQueue();
  }

  void Texture2D::Bind(u32 unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, TextureStore::GetRendererID(texture_id));
  }

  u32 TextureStore::GetTextureID(const std::filesystem::path &path) {
    if (_path_to_id.count(path))
      return _path_to_id[path];
    return 0;
  }

  u32 TextureStore::GetRendererID(u32 id) {
    if (_data.count(id))
      return _data[id].gl_id;
    return 0;
  }

  void TextureStore::RegisterTexture(Texture2D &texture, const std::filesystem::path &path,
                                     TextureType type, const TextureData &data)
  {
    if (!path.empty() && _path_to_id.count(path)) {
      texture.texture_id = _path_to_id[path];
      _data[texture.texture_id].instances++;
      return;
    }

    _id_counter++;
    texture.texture_id = _id_counter;
    texture.type = type;
    if (!path.empty())
      _path_to_id.insert(std::pair<std::filesystem::path, u32>(path, texture.texture_id));
    _data.insert(std::pair<u32, TextureData>(_id_counter, data));
    _data[texture.texture_id].instances++;
    _texture_2d_queue.emplace(texture);
  }

  void TextureStore::RegisterTexture(TextureCube &texture, const std::filesystem::path &path,
                                     TextureType type, const TextureData &data)
  {
    if (!path.empty() && _path_to_id.count(path)) {
      texture.texture_id = _path_to_id[path];
      _data[texture.texture_id].instances++;
      return;
    }

    _id_counter++;
    texture.texture_id = _id_counter;
    texture.type = type;
    if (!path.empty())
      _path_to_id.insert(std::pair<std::filesystem::path, u32>(path, texture.texture_id));
    _data.insert(std::pair<u32, TextureData>(_id_counter, data));
    _data[texture.texture_id].instances++;
    _data[texture.texture_id].cubemap = true;
    _texture_cube_queue.emplace(texture);
  }

  std::filesystem::path TextureStore::GetPath(u32 id) {
    for (auto it = _path_to_id.cbegin(); it != _path_to_id.cend(); ++it) {
      if (it->second == id)
        return it->first;
    }
    return "";
  }

  void TextureStore::ProcessQueue() {
    while (!_texture_2d_queue.empty()) {
      Texture2D &t = _texture_2d_queue.front();
      std::filesystem::path path = GetPath(t.texture_id);
      log::debug("Processing texture 2d: {}", path.string());
      if (!path.empty()) {
        LoadTexture(t, path);
      } else {
        CreateTexture(t);
      }
      _texture_2d_queue.pop();
    }

    while (!_texture_cube_queue.empty()) {
      TextureCube &t = _texture_cube_queue.front();
      std::filesystem::path path = GetPath(t.texture_id);
      log::debug("Processing texture cube: {}", path.string());
      if (!path.empty())
        LoadCubemap(t, path);
      _texture_cube_queue.pop();
    }
  }

  void TextureStore::LoadCubemap(const TextureCube &texture, const std::filesystem::path &path) {
    std::vector<std::filesystem::path> paths;
    paths.push_back(std::filesystem::path(path.string() + "_right.jpg"));
    paths.push_back(std::filesystem::path(path.string() + "_left.jpg"));
    paths.push_back(std::filesystem::path(path.string() + "_top.jpg"));
    paths.push_back(std::filesystem::path(path.string() + "_bottom.jpg"));
    paths.push_back(std::filesystem::path(path.string() + "_back.jpg"));
    paths.push_back(std::filesystem::path(path.string() + "_front.jpg"));

    glGenTextures(1, &_data[texture.texture_id].gl_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _data[texture.texture_id].gl_id);

    stbi_set_flip_vertically_on_load(false);
    for (u32 i = 0; i < 6; i++) {
      i32 width, height, channels;
      u8 *data = stbi_load(paths[i].string().c_str(), &width, &height, &channels, 0);
      if (!data) {
        log::error("Failed to load cubemap texture: {}", paths[i].string());
        continue;
      }

      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);

      log::debug("Loaded cubemap texture: {}", paths[i].string());
    }
    stbi_set_flip_vertically_on_load(true);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    _data[texture.texture_id].loaded = true;
  }

  void TextureStore::LoadTexture(const Texture2D &texture, const std::filesystem::path &path) {
    log::debug("Loading Texture \"{}\", type {}", path.string(), Texture2D::TextureTypeToString(texture.type));

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
    _data[texture.texture_id].loaded = true;

    stbi_image_free(data);
  }

  void TextureStore::CreateTexture(const Texture2D &texture) {
    TextureData &data = _data[texture.texture_id];
    if (data.size.x <= 0 || data.size.y <= 0) {
      log::debug("Texture \"{}\" has invalid data.size {}x{}", Texture2D::TextureTypeToString(texture.type), data.size.x, data.size.y);
      _data[texture.texture_id].loaded = false;
      return;
    }

    u32 internal_format = 0;
    switch (data.internal_format) {
      case TextureInternalFormat::RGB:
        internal_format = GL_RGB;
        break;
      case TextureInternalFormat::RGBA:
        internal_format = GL_RGBA;
        break;
      case TextureInternalFormat::DepthStencil:
        internal_format = GL_DEPTH_STENCIL;
        break;
      case TextureInternalFormat::Depth:
        internal_format = GL_DEPTH_COMPONENT;
        break;
      case TextureInternalFormat::Last:
        break;
    }

    u32 format = 0;
    switch (data.format) {
      case TextureFormat::RGB:
        format = GL_RGB;
        break;
      case TextureFormat::RGBA:
        format = GL_RGBA;
        break;
      case TextureFormat::Depth:
        format = GL_DEPTH_COMPONENT32F;
        break;
      case TextureFormat::DepthStencil:
        format = GL_DEPTH24_STENCIL8;
        break;
      case TextureFormat::Stencil:
        format = GL_STENCIL_INDEX;
        break;
      case TextureFormat::Last:
        break;
    }

    u32 type = 0;
    switch (data.data_type) {
      case TextureDataType::U8:
        type = GL_UNSIGNED_BYTE;
        break;
      case TextureDataType::U16:
        type = GL_UNSIGNED_SHORT;
        break;
      case TextureDataType::U24_8:
        type = GL_UNSIGNED_INT_24_8;
        break;
      case TextureDataType::U32:
        type = GL_UNSIGNED_INT;
        break;
      case TextureDataType::I8:
        type = GL_BYTE;
        break;
      case TextureDataType::I16:
        type = GL_SHORT;
        break;
      case TextureDataType::I32:
        type = GL_INT;
        break;
      case TextureDataType::F16:
        type = GL_HALF_FLOAT;
        break;
      case TextureDataType::F32:
        type = GL_FLOAT;
        break;
      case TextureDataType::Last:
        break;
    }

    u32 error = glGetError();
    while (error != GL_NO_ERROR) {
      log::error("Cleaning OpenGL error: {}", error);
      error = glGetError();
    }

    u32 tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, data.size.x, data.size.y, 0, internal_format, type, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    error = glGetError();
    while (error != GL_NO_ERROR) {
      log::error("OpenGL error: {}", error);
      error = glGetError();
    }

    _data[texture.texture_id].gl_id = tex;
    _data[texture.texture_id].loaded = true;

    log::debug("Created Texture size {}x{} id {}", data.size.x, data.size.y, tex);
  }

  TextureData & TextureStore::GetData(u32 id) {
    return _data[id];
  }

  void TextureStore::DeregisterTexture(u32 id) {
    if (!_data.count(id)) {
      log::error("Texture id {} not registered", id);
      return;
    }

    _data[id].instances--;
    if (_data[id].instances > 0)
      return;

    log::debug("Deleting Texture \"{}\"", GetPath(id).string());

    if (_data[id].gl_id != 0)
      glDeleteTextures(1, &_data[id].gl_id);

    _data.erase(id);
    _path_to_id.erase(GetPath(id));
  }

  Texture2D::operator u32() const {
    return TextureStore::GetRendererID(texture_id); // renderer_id
  }

} // namespace axl
