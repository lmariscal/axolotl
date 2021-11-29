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

    bool Bind(u32 id = 0, u32 unit = 0, u32 count = 0, TextureType type = TextureType::Last);
    void BindAll();
    void Build();
    void AddTexture(const std::filesystem::path &path, TextureType type = TextureType::Last);
    void AddTexture(u32 id, TextureType type = TextureType::Last);
    Shader & GetShader();

    protected:
     std::shared_ptr<Shader> _shader;
     std::shared_ptr<std::array<std::vector<Texture *>, (i32)TextureType::Last>> _textures;
     std::set<std::filesystem::path> _textures_path;
  };

} // namespace axl
