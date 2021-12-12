#pragma once

#include <array>
#include <assimp/scene.h>
#include <axolotl/component.hh>
#include <axolotl/material.hh>
#include <axolotl/mesh.hh>
#include <axolotl/types.hh>
#include <vector>

namespace axl {

  class Model {
   public:
    Model(std::filesystem::path path = "", std::array<std::string, (i32)ShaderType::Last> paths = {}, bool root = true);
    ~Model();

    void Draw(Material &material);

    json Serialize() const;
    void Deserialize(const json &json);
    bool ShowComponent();
    void Init();

    bool two_sided;

    REGISTER_COMPONENT(Model, _path, _root)

   protected:
    static void ProcessNode(Ento ento, Model &model, aiNode *node, const aiScene *scene);
    static Mesh *ProcessMesh(Ento ento, Model &model, aiMesh *mesh, const aiScene *scene);
    static void ProcessMaterialTextures(Ento ento, Model &model, aiMaterial *material, aiTextureType type);

    std::shared_ptr<std::vector<Mesh *>> _meshes;
    std::filesystem::path _path;
    std::array<std::string, (i32)ShaderType::Last> _shader_paths;
    u32 _mesh_id;
    bool _root = true;
  };

} // namespace axl
