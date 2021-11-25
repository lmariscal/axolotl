#pragma once

#include <axolotl/types.h>
#include <axolotl/component.h>
#include <axolotl/texture.h>
#include <axolotl/shader.h>

namespace axl {

  struct Material : public Component {
    Material(const ShaderPaths &paths);
    ~Material();

    virtual json Serialize() const;
    virtual void Deserialize(const json &json);
    virtual bool ShowData();
    virtual void Init();

    void Bind();
    void Build();
    void AddTexture(const std::filesystem::path &path, TextureType type = TextureType::Last);
    void AddTexture(u32 id, TextureType type = TextureType::Last);
    Shader * GetShader();

    protected:
     Shader *_shader;
     std::vector<Texture *> _textures;
     std::set<std::filesystem::path> _textures_path;
  };

} // namespace axl
