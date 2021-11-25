#include <axolotl/material.h>

#include <axolotl/scene.h>
#include <axolotl/shader.h>
#include <axolotl/texture.h>

namespace axl {

  Material::Material(const ShaderPaths &paths):
    _shader(nullptr)
  {
    _shader = new Shader(paths);
  }

  Material::~Material() {
    delete _shader;
    for (Texture *texture : _textures)
      delete texture;
  }

  void Material::Init() {
    _shader->Init();
    _shader->Compile();
  }

  void Material::Bind() {
    _shader->Bind();

    u32 counter[(i32)TextureType::Last];
    std::fill(counter, counter + (i32)TextureType::Last, 0);
    i32 i = 0;
    for (Texture *texture : _textures) {
      if (texture->type == TextureType::Last)
        continue;

      u32 index = ++counter[(i32)texture->type];
      std::string name = Texture::TextureTypeToString(texture->type) + std::to_string(counter[(i32)texture->type]);
      // _shader->SetUniformTexture(i, TextureStore::GetRendererTextureID(texture->texture_id));
      _shader->SetUniformI32(name, i);
      texture->Bind(i);
      i++;
    }
  }

  void Material::AddTexture(const std::filesystem::path &path, TextureType type) {
    if (_textures_path.count(path))
      return;

    Texture *texture = new Texture(path, type);
    _textures.push_back(texture);
    _textures_path.insert(path);
    log::debug("Added texture {}", path.string());
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
