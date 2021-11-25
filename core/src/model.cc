#include <axolotl/model.h>

#include <axolotl/material.h>
#include <axolotl/scene.h>
#include <axolotl/mesh.h>
#include <axolotl/transform.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace axl {

  Model::Model(const std::filesystem::path &path, const ShaderPaths &paths, bool root):
    _path(path),
    _shader_paths(paths),
    _root(root)
  {
  }

  Model::~Model() {
    for (Mesh *mesh : _meshes)
      delete mesh;
  }

  void Model::ProcessMaterialTextures(aiMaterial *ai_material, aiTextureType ai_type, const aiScene *scene, Model *model) {
    TextureType type = TextureType::Last;
    switch (ai_type) {
      case aiTextureType_DIFFUSE:
        type = TextureType::Diffuse;
        break;
      case aiTextureType_SPECULAR:
        type = TextureType::Specular;
        break;
      case aiTextureType_AMBIENT:
        type = TextureType::Ambient;
        break;
      case aiTextureType_NORMALS:
        type = TextureType::Normals;
        break;
      case aiTextureType_HEIGHT:
        type = TextureType::Height;
        break;
      default:
        break;
    }

    Material &material = model->_scene->GetComponent<Material>(*model->_parent);
    for (unsigned int i = 0; i < ai_material->GetTextureCount(ai_type); ++i) {
      aiString path;
      ai_material->GetTexture(ai_type, i, &path);

      std::filesystem::path full_path = model->_path.parent_path() / path.C_Str();
      material.AddTexture(full_path, type);
    }
  }

  Mesh * Model::ProcessMesh(aiMesh *mesh, const aiScene *scene, Model *model) {
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

    ProcessMaterialTextures(material, aiTextureType_DIFFUSE, scene, model);
    ProcessMaterialTextures(material, aiTextureType_SPECULAR, scene, model);
    ProcessMaterialTextures(material, aiTextureType_HEIGHT, scene, model);
    ProcessMaterialTextures(material, aiTextureType_AMBIENT, scene, model);

    return new Mesh(buffer_data, indices);
  }

  void Model::ProcessNode(aiNode *node, const aiScene *scene, Model *model) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      model->_meshes.push_back(ProcessMesh(mesh, scene, model));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      entt::entity child = model->_scene->CreateEntity();
      model->_scene->AddComponent<Transform>(child);
      Model *child_model = &model->_scene->AddComponent<Model>(child, model->_path, model->_shader_paths, false);
      model->_parent->AddChild(child_model->_parent);
      model->_parent->name = node->mName.C_Str();
      log::debug("Processing node {}", node->mName.C_Str());
      ProcessNode(node->mChildren[i], scene, child_model);
    }
  }

  void Model::Draw() {
    for (Mesh *mesh : _meshes)
      mesh->Draw();
  }

  void Model::Init() {
    _scene->TryAddComponent<Material>(*_parent, _shader_paths);

    if (!_root)
      return;

    log::debug("Loading model from {}", _path.string());
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(_path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      log::error("Assimp error: {}", importer.GetErrorString());
      return;
    }

    ProcessNode(scene->mRootNode, scene, this);
  }

  json Model::Serialize() const {
    json j = GetRootNode("model");
    return j;
  }

  void Model::Deserialize(const json &j) {
  }

  bool Model::ShowData() {
    return false;
  }

} // namespace axl
