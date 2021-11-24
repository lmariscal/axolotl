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
    void SetShader(const std::filesystem::path &path);
    void AddTexture(const std::filesystem::path &path);
    void AddTexture(u32 id);

    // protected:
     Shader *_shader;
     std::vector<Texture> _textures;

     std::filesystem::path _vertex_shader_path;
     std::filesystem::path _fragment_shader_path;
  };

} // namespace axl
