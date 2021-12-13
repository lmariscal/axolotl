#pragma once

#include <array>
#include <axolotl/component.hh>
#include <axolotl/shader.hh>
#include <axolotl/texture.hh>
#include <axolotl/types.hh>

namespace axl {

  class Material {
   public:
    Material(const std::vector<std::string> &paths);
    ~Material();

    virtual json Serialize() const;
    virtual void Deserialize(const json &json);
    virtual bool ShowData();

    void Init();

    bool Bind(u32 unit = 0, TextureType type = TextureType::Last);
    void BindAll();
    void Build();
    void AddTexture(const std::filesystem::path &path, TextureType type = TextureType::Last);
    void AddTexture(u32 id, TextureType type = TextureType::Last);
    Shader &GetShader();

   protected:
    std::shared_ptr<Shader> _shader;
    std::array<Texture2D, (i32)TextureType::Last> _textures;
    std::set<std::filesystem::path> _textures_path;
  };

} // namespace axl
