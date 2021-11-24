#include <axolotl/material.h>

#include <axolotl/scene.h>
#include <axolotl/shader.h>
#include <axolotl/texture.h>

#include <glad.h>

namespace axl {

  Material::Material(const ShaderPaths &paths):
    _shader(nullptr)
  {
    _shader = new Shader(paths);
  }

  Material::~Material() {
    delete _shader;
  }

  void Material::Init() {
    _shader->Init();
    _shader->Compile();
  }

  void Material::Bind() {
    _shader->Bind();
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
