#include <axolotl/model.h>

#include <axolotl/scene.h>
#include <axolotl/mesh.h>
#include <axolotl/material.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace axl {

  Model::Model(const std::filesystem::path &path):
    _path(path)
  {
    log::debug("Loading model from {}", path.string());

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.string(),
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      log::error("Assimp error: {}", importer.GetErrorString());
      return;
    }

    ProcessNode(scene->mRootNode, scene);
  }

  Model::~Model() {
    for (Mesh *mesh : _meshes)
      delete mesh;
  }

  Mesh * Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) {
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

    return new Mesh(buffer_data, indices);
  }

  void Model::ProcessNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      _meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
      ProcessNode(node->mChildren[i], scene);
  }

  void Model::Draw() {
    for (Mesh *mesh : _meshes)
      mesh->Draw();
  }

  void Model::Init() {
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
