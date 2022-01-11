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
    Model(std::filesystem::path path = "", std::vector<std::string> paths = {}, bool root = true);
    ~Model();

    void Draw();

    json Serialize() const;
    void Deserialize(const json &json);
    bool ShowComponent();
    void Init();

    std::filesystem::path SolvePath(const std::filesystem::path &path) const;

    bool two_sided;

    REGISTER_COMPONENT(Model, _path, _root, _shader_paths, _mesh_id)

   protected:
    friend class Renderer;
    friend class Scene;

    static void ProcessNode(Ento ento, Model &model, aiNode *node, const aiScene *scene);
    static Mesh *ProcessMesh(Ento ento, Model &model, aiMesh *mesh, const aiScene *scene);
    static void ProcessMaterialTextures(u32 index, Model &model, aiMaterial *material, aiTextureType type);

    std::shared_ptr<std::vector<Mesh *>>
      _meshes; // TODO: Replace with vector of unique_ptr, and make a resource manager, this solution sucks
    std::shared_ptr<std::unordered_map<u32, std::unique_ptr<Material>>>
      _materials; // TODO: Move material from component to model
    std::filesystem::path _path;
    std::vector<std::string> _shader_paths;
    u32 _mesh_id;
    bool _root = true;
  };

} // namespace axl
