#pragma once

#include <axolotl/types.h>
#include <axolotl/component.h>
#include <axolotl/mesh.h>

#include <vector>

struct aiNode;
struct aiMesh;
struct aiScene;

namespace axl {

  struct Model : public Component {
    Model(const std::filesystem::path &path);
    ~Model();

    void Draw();

    virtual json Serialize() const;
    virtual void Deserialize(const json &json);
    virtual bool ShowData();
    virtual void Init();

   protected:
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh * ProcessMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Mesh *> _meshes;
    std::filesystem::path _path;
  };

} // namespace axl
