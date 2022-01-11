#include <axolotl/material.hh>
#include <axolotl/scene.hh>
#include <axolotl/shader.hh>
#include <axolotl/texture.hh>

namespace axl {

  Material::Material(const std::vector<std::string> &paths):
    _textures(std::array<Texture2D, (i32)TextureType::Last>()) {

    std::fill(_textures.begin(), _textures.end(), Texture2D());
    std::array<std::string, (i32)ShaderType::Last> shader_paths;
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      shader_paths[i] = paths.size() > i ? paths[i] : "";
      shader_paths[i] = ShaderStore::SolvePath(shader_paths[i]);
    }
    _shader = std::make_shared<Shader>(ShaderData(shader_paths[(i32)ShaderType::Vertex],
                                                  shader_paths[(i32)ShaderType::Fragment],
                                                  shader_paths[(i32)ShaderType::Geometry],
                                                  shader_paths[(i32)ShaderType::Compute]));
  }

  void Material::Init() {
    ShaderStore::ProcessQueue();
  }

  Material::~Material() { }

  void Material::BindAll() {
    _shader->Bind();
    for (i32 i = 1; i < (i32)TextureType::Last; ++i) {
      Bind(i, (TextureType)i);
    }
  }

  bool Material::Bind(u32 unit, TextureType type) {
    if (type == TextureType::Last)
      return false;

    if (!_textures[(i32)type].texture_id) {
      _shader->SetUniformTexture(type, -1);
      return false;
    }

    Texture2D &texture = _textures[(i32)type];
    texture.Bind(unit);
    _shader->SetUniformTexture(type, unit);
    return true;
  }

  void Material::AddTexture(const std::filesystem::path &path, TextureType type) {
    if (_textures_path.count(path))
      return;

    _textures[(i32)type] = Texture2D(path, type);
    _textures_path.insert(path);
    log::debug("Added texture {} of type {}", path.string(), Texture2D::TextureTypeToString(type));
  }

  void Material::AddTexture(u32 id, TextureType type) {
    std::filesystem::path path = TextureStore::GetPath(id);
    AddTexture(path, type);
  }

  Shader &Material::GetShader() {
    return *_shader;
  }

  json Material::Serialize() const {
    json j;
    return j;
  }

  void Material::Deserialize(const json &j) { }

  bool Material::ShowData() {
    // _shader->ShowData();
    return false;
  }

} // namespace axl
