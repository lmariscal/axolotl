#include <axolotl/model.h>

#include <axolotl/material.h>
#include <axolotl/scene.h>
#include <axolotl/mesh.h>
#include <axolotl/transform.h>
#include <axolotl/ento.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace axl {

  Model::Model(Ento ento, const std::filesystem::path &path, const ShaderPaths &paths, bool root):
    _path(path),
    _shader_paths(paths),
    _root(root),
    _meshes(std::make_shared<std::vector<Mesh *>>())
  {
    ento.TryAddComponent<Material>(_shader_paths);

    if (!_root)
      return;

    log::debug("Loading model from {}", _path.string());
    Assimp::Importer importer;

    u32 flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs;
    if (_path.extension().string() == ".gltf" || _path.extension().string() == ".glb")
      flags &= ~aiProcess_FlipUVs;
    const aiScene *scene = importer.ReadFile(_path.string(), flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      log::error("Assimp error: {}", importer.GetErrorString());
      return;
    }

    aiVector3D scale, position;
    aiQuaternion rotation;
    scene->mRootNode->mTransformation.Decompose(scale, rotation, position);
    Transform &transform = ento.TryAddComponent<Transform>();
    transform.SetPosition(v3(position.x, position.y, position.z));
    transform.SetRotation(quat(rotation.w, rotation.x, rotation.y, rotation.z));
    transform.SetScale(v3(scale.x, scale.y, scale.z));

    ProcessNode(ento, scene->mRootNode, scene);

    TextureStore::ProcessQueue();
  }

  Model::~Model() {
    if (_meshes.use_count() > 1)
      return;
    for (Mesh *mesh : *_meshes)
      delete mesh;
  }

  void Model::ProcessMaterialTextures(Ento ento, aiMaterial *ai_material, aiTextureType ai_type) {
    Model &model = ento.GetComponent<Model>();
    TextureType type = TextureType::Last;
    switch (ai_type) {
      case aiTextureType_DIFFUSE:
        type = TextureType::Diffuse;
        break;
      case aiTextureType_SPECULAR:
        type = TextureType::Specular;
        break;
      case aiTextureType_NORMALS:
        type = TextureType::Normal;
        break;
      case aiTextureType_AMBIENT:
        type = TextureType::Ambient;
        break;
      case aiTextureType_HEIGHT:
        type = TextureType::Height;
        break;
      default:
        break;
    }

    Material &material = ento.GetComponent<Material>();
    for (unsigned int i = 0; i < ai_material->GetTextureCount(ai_type); ++i) {
      aiString path;
      ai_material->GetTexture(ai_type, i, &path);

      std::filesystem::path full_path = model._path.parent_path() / path.C_Str();
      material.AddTexture(full_path, type);
    }
  }

  Mesh * Model::ProcessMesh(Ento ento, aiMesh *mesh, const aiScene *scene) {
    std::vector<f32> buffer_data;
    std::vector<u32> indices;

    for (u32 i = 0; i < mesh->mNumVertices; i++) {
      buffer_data.push_back(mesh->mVertices[i].x);
      buffer_data.push_back(mesh->mVertices[i].y);
      buffer_data.push_back(mesh->mVertices[i].z);

      buffer_data.push_back(mesh->mNormals[i].x);
      buffer_data.push_back(mesh->mNormals[i].y);
      buffer_data.push_back(mesh->mNormals[i].z);

      if (mesh->mTextureCoords[0]) {
        buffer_data.push_back(mesh->mTextureCoords[0][i].x);
        buffer_data.push_back(mesh->mTextureCoords[0][i].y);
      } else {
        buffer_data.push_back(0.0f);
        buffer_data.push_back(0.0f);
      }
    }

    for (u32 i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      for (u32 j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    Model &model = ento.GetComponent<Model>();

    ProcessMaterialTextures(ento, material, aiTextureType_DIFFUSE);
    ProcessMaterialTextures(ento, material, aiTextureType_SPECULAR);
    ProcessMaterialTextures(ento, material, aiTextureType_AMBIENT);
    ProcessMaterialTextures(ento, material, aiTextureType_NORMALS);
    ProcessMaterialTextures(ento, material, aiTextureType_HEIGHT);

    return new Mesh(buffer_data, indices);
  }

  void Model::ProcessNode(Ento ento, aiNode *node, const aiScene *scene) {
    Model &model = ento.GetComponent<Model>();
    ento.Tag().value = node->mName.C_Str();

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      Mesh *m = ProcessMesh(ento, mesh, scene);

      if (node->mNumMeshes > 1)
        m->_single_mesh = false;

      model._meshes->push_back(m);
      m->SetMaterialID(mesh->mMaterialIndex);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      Ento child = Scene::GetActiveScene()->CreateEntity();

      Transform &transform = child.GetComponent<Transform>();
      Model &child_model = child.AddComponent<Model>(child, model._path, model._shader_paths, false);
      child.SetParent(ento);

      // aiVector3D scale, position;
      // aiQuaternion rotation;
      // node->mTransformation.Decompose(scale, rotation, position);
      // transform.SetPosition(v3(position.x, position.y, position.z));
      // transform.SetRotation(quat(rotation.w, rotation.x, rotation.y, rotation.z));
      // transform.SetScale(v3(scale.x, scale.y, scale.z));

      log::debug("Processing node {}", node->mName.C_Str());
      ProcessNode(child, node->mChildren[i], scene);
    }
  }

  void Model::Draw(Material &material) {
    for (Mesh *mesh : *_meshes) {
      u32 material_id = mesh->GetMaterialID();
      if (!mesh->_single_mesh) {
        i32 unit_count = 0;
        for (i32 i = 0; i < (i32)TextureType::Last; ++i) {
          if (material.Bind(material_id, unit_count, 0, (TextureType)i))
            unit_count++;
        }
      } else {
        material.BindAll();
      }
      mesh->Draw();
    }
  }

  void Model::Init() {
  }

  json Model::Serialize() const {
    json j;
    return j;
  }

  void Model::Deserialize(const json &j) {
  }

  bool Model::ShowData() {
    return false;
  }

} // namespace axl
