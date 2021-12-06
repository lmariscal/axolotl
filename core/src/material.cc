#include <axolotl/material.hh>

#include <axolotl/scene.hh>
#include <axolotl/shader.hh>
#include <axolotl/texture.hh>

namespace axl {

  Material::Material(const std::array<std::string, (i32)ShaderType::Last> &paths):
    _shader(std::make_shared<Shader>(ShaderData(
      paths[(i32)ShaderType::Vertex],
      paths[(i32)ShaderType::Fragment],
      paths[(i32)ShaderType::Geometry],
      paths[(i32)ShaderType::Compute]
    ))),
    _textures(std::make_shared<std::array<std::vector<Texture2D *>, (i32)TextureType::Last>>())
  {
    std::fill(_textures->begin(), _textures->end(), std::vector<Texture2D *>());

    ShaderStore::ProcessQueue();
  }

  Material::~Material() {
    log::debug("Material destroyed, texture use cound is {}", _textures.use_count());
    if (_textures.use_count() > 1)
      return;

    log::debug("Destroying textures");

    for (i32 i = 0; i < (i32)TextureType::Last; ++i)
      for (Texture2D *texture : (*_textures)[i])
        delete texture;
  }

  void Material::BindAll() {
    _shader->Bind();

    u32 counter[(i32)TextureType::Last];
    std::fill(counter, counter + (i32)TextureType::Last, 0);

    u32 unit_count = 1;
    for (i32 i = 1; i < (i32)TextureType::Last; ++i) {
      std::vector<Texture2D *> &textures = (*_textures)[i];
      for (i32 j = 0; j < textures.size(); ++j) {
        u32 count = counter[i]++;
        if (Bind(j, unit_count, count, (TextureType)i))
          unit_count++;
      }

      if (textures.empty())
        _shader->SetUniformTexture((TextureType)i, -1);
    }
  }

  bool Material::Bind(u32 id, u32 unit, u32 count, TextureType type) {
    if (type == TextureType::Last)
      return false;

    if ((*_textures)[(i32)type].empty() || id >= (*_textures)[(i32)type].size()) {
      _shader->SetUniformTexture(type, -1);
      return false;
    }

    auto &t = (*_textures)[(i32)type];
    Texture2D *texture = t[id];
    texture->Bind(unit);
    _shader->SetUniformTexture(type, unit);
    return true;
  }

  void Material::AddTexture(const std::filesystem::path &path, TextureType type) {
    if (_textures_path.count(path))
      return;

    Texture2D *texture = new Texture2D(path, type);
    (*_textures)[(i32)type].push_back(texture);
    _textures_path.insert(path);
    log::debug("Added texture {} of type {}", path.string(), Texture2D::TextureTypeToString(type));
  }

  void Material::AddTexture(u32 id, TextureType type) {
    std::filesystem::path path = TextureStore::GetPath(id);
    AddTexture(path, type);
  }

  Shader & Material::GetShader() {
    return *_shader;
  }

  json Material::Serialize() const {
    json j;
    return j;
  }

  void Material::Deserialize(const json &j) {
  }

  bool Material::ShowData() {
    // _shader->ShowData();
    return false;
  }

} // namespace axl
