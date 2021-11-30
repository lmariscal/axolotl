#pragma once

#include <axolotl/types.h>
#include <axolotl/component.h>
#include <axolotl/mesh.h>
#include <axolotl/material.h>

#include <assimp/scene.h>

#include <vector>

namespace axl {

  struct Model {
    Model(Ento ento, std::filesystem::path path = "", ShaderPaths paths = { }, bool root = true);
    ~Model();

    void Draw(Material &material);

    virtual json Serialize() const;
    virtual void Deserialize(const json &json);
    virtual bool ShowData();
    void Init(Ento &ento);

   protected:
    static void ProcessNode(Ento ento, Model &model, aiNode *node, const aiScene *scene);
    static Mesh * ProcessMesh(Ento ento, Model &model, aiMesh *mesh, const aiScene *scene);
    static void ProcessMaterialTextures(Ento ento, Model &model, aiMaterial *material, aiTextureType type);

    std::shared_ptr<std::vector<Mesh *>> _meshes;
    std::filesystem::path _path;
    ShaderPaths _shader_paths;
    bool _root = true;
  };

} // namespace axl
