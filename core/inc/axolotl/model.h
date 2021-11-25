#pragma once

#include <axolotl/types.h>
#include <axolotl/component.h>
#include <axolotl/mesh.h>
#include <axolotl/material.h>

#include <assimp/scene.h>

#include <vector>

namespace axl {

  struct Model : public Component {
    Model(const std::filesystem::path &path = "", const ShaderPaths &paths = { }, bool root = true);
    ~Model();

    void Draw();

    virtual json Serialize() const;
    virtual void Deserialize(const json &json);
    virtual bool ShowData();
    virtual void Init();

   protected:
    static void ProcessNode(aiNode *node, const aiScene *scene, Model *model);
    static Mesh * ProcessMesh(aiMesh *mesh, const aiScene *scene, Model *model);
    static void ProcessMaterialTextures(aiMaterial *material, aiTextureType type, const aiScene *scene, Model *model);

    std::vector<Mesh *> _meshes;
    std::filesystem::path _path;
    ShaderPaths _shader_paths;
    bool _root = true;
  };

} // namespace axl
