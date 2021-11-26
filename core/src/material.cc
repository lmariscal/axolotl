#include <axolotl/material.h>

#include <axolotl/scene.h>
#include <axolotl/shader.h>
#include <axolotl/texture.h>

namespace axl {

  Material::Material(const ShaderPaths &paths):
    _shader(nullptr)
  {
    for (i32 i = 0; i < (i32)TextureType::Last; ++i)
      _textures[i] = { };

    _shader = new Shader(paths);
  }

  Material::~Material() {
    delete _shader;
    for (i32 i = 0; i < (i32)TextureType::Last; ++i)
      for (Texture *texture : _textures[i])
        delete texture;
  }

  void Material::Init() {
    _shader->Init();
    _shader->Compile();
  }

  void Material::BindAll() {
    _shader->Bind();

    u32 counter[(i32)TextureType::Last];
    std::fill(counter, counter + (i32)TextureType::Last, 0);

    u32 unit_count = 0;
    for (i32 i = 0; i < (i32)TextureType::Last; ++i) {
      for (i32 j = 0; j < _textures[i].size(); ++j) {
        u32 count = counter[i]++;
        if (Bind(j, unit_count, count, (TextureType)i))
          unit_count++;
      }
    }
  }

  bool Material::Bind(u32 id, u32 unit, u32 count, TextureType type) {
    if (type == TextureType::Last || _textures[(i32)type].empty() || id >= _textures[(i32)type].size())
      return false;

    auto &t = _textures[(i32)type];
    Texture *texture = t[id];
    std::string name = Texture::TextureTypeToString(type) + std::to_string(count + 1);
    _shader->SetUniformI32(name, unit);
    _shader->SetUniformTexture(unit, TextureStore::GetRendererTextureID(texture->texture_id));
    return true;
  }

  void Material::AddTexture(const std::filesystem::path &path, TextureType type) {
    if (_textures_path.count(path))
      return;

    Texture *texture = new Texture(path, type);
    _textures[(i32)type].push_back(texture);
    _textures_path.insert(path);
    log::debug("Added texture {} of type {}", path.string(), Texture::TextureTypeToString(type));
  }

  void Material::AddTexture(u32 id, TextureType type) {
    std::filesystem::path path = TextureStore::GetPath(id);
    AddTexture(path, type);
  }

  Shader * Material::GetShader() {
    return _shader;
  }

  json Material::Serialize() const {
    json j = GetRootNode("material");
    return j;
  }

  void Material::Deserialize(const json &j) {
  }

  bool Material::ShowData() {
    _shader->ShowData();
    return false;
  }

} // namespace axl
