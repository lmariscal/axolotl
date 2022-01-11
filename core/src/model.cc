#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <axolotl/axolotl.hh>
#include <axolotl/ento.hh>
#include <axolotl/material.hh>
#include <axolotl/mesh.hh>
#include <axolotl/model.hh>
#include <axolotl/scene.hh>
#include <axolotl/transform.hh>
#include <glad.h>

namespace axl {

  Model::Model(std::filesystem::path path, std::vector<std::string> paths, bool root):
    _path(path),
    _shader_paths(paths),
    _root(root),
    _mesh_id(0),
    _materials(std::make_shared<std::unordered_map<u32, std::unique_ptr<Material>>>()),
    _meshes(std::make_shared<std::vector<Mesh *>>()) { }

  Model::~Model() {
    if (_meshes.use_count() > 1)
      return;
    for (Mesh *mesh : *_meshes)
      delete mesh;
  }

  std::filesystem::path Model::SolvePath(const std::filesystem::path &path) const {
    std::string dist_dir = Axolotl::GetDistDir();
    std::string dist_dir_str = "${DistDir}";
    std::string path_str = path.string();
    std::string::size_type pos = path_str.find(dist_dir_str);

    if (pos != std::string::npos) {
      path_str.replace(pos, dist_dir_str.length(), dist_dir);
      return std::filesystem::path(path_str);
    }

    return path;
  }

  void Model::Init() {
    Ento ento = Ento::FromComponent(*this);

    if (!_root)
      return;

    _path = SolvePath(_path);

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
    ProcessNode(ento, *this, scene->mRootNode, scene);
    TextureStore::ProcessQueue();
  }

  void Model::ProcessMaterialTextures(u32 index, Model &model, aiMaterial *ai_material, aiTextureType ai_type) {
    TextureType type = TextureType::Last;
    switch (ai_type) {
      case aiTextureType_DIFFUSE:
        type = TextureType::Diffuse;
        break;
      case aiTextureType_SPECULAR:
        type = TextureType::Specular;
        break;
      case aiTextureType_EMISSIVE:
        type = TextureType::Specular; // Make it emissive
        break;
      case aiTextureType_NORMALS:
        type = TextureType::Normal;
        break;
      case aiTextureType_AMBIENT:
        type = TextureType::Ambient;
        break;
      case aiTextureType_HEIGHT:
        type = TextureType::Normal;
        break;
      default:
        break;
    }

    if (!model._materials->count(index))
      (*model._materials)[index] = std::make_unique<Material>(model._shader_paths);

    Material &material = *(*model._materials)[index];
    for (unsigned int i = 0; i < ai_material->GetTextureCount(ai_type); ++i) {
      aiString path;
      ai_material->GetTexture(ai_type, i, &path);

      std::filesystem::path full_path = model._path.parent_path() / path.C_Str();
      material.AddTexture(full_path, type);
    }
  }

  Mesh *Model::ProcessMesh(Ento ento, Model &model, aiMesh *mesh, const aiScene *scene) {
    std::vector<f32> buffer_data;
    std::vector<u32> indices;

    for (u32 i = 0; i < mesh->mNumVertices; i++) {
      buffer_data.push_back(mesh->mVertices[i].x);
      buffer_data.push_back(mesh->mVertices[i].y);
      buffer_data.push_back(mesh->mVertices[i].z);

      buffer_data.push_back(mesh->mNormals[i].x);
      buffer_data.push_back(mesh->mNormals[i].y);
      buffer_data.push_back(mesh->mNormals[i].z);

      if (mesh->HasTangentsAndBitangents()) {
        buffer_data.push_back(mesh->mTangents[i].x);
        buffer_data.push_back(mesh->mTangents[i].y);
        buffer_data.push_back(mesh->mTangents[i].z);
      } else {
        buffer_data.push_back(0.0f);
        buffer_data.push_back(0.0f);
        buffer_data.push_back(0.0f);
      }

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

    ProcessMaterialTextures(mesh->mMaterialIndex, model, material, aiTextureType_DIFFUSE);
    ProcessMaterialTextures(mesh->mMaterialIndex, model, material, aiTextureType_EMISSIVE);
    ProcessMaterialTextures(mesh->mMaterialIndex, model, material, aiTextureType_SPECULAR);
    ProcessMaterialTextures(mesh->mMaterialIndex, model, material, aiTextureType_AMBIENT);
    ProcessMaterialTextures(mesh->mMaterialIndex, model, material, aiTextureType_NORMALS);
    ProcessMaterialTextures(mesh->mMaterialIndex, model, material, aiTextureType_HEIGHT);

    Mesh *result = new Mesh(buffer_data, indices);

    return result;
  }

  void Model::ProcessNode(Ento ento, Model &model, aiNode *node, const aiScene *scene) {
    if (ento.Tag().value == Tag::DefaultTag)
      ento.Tag().value = node->mName.C_Str();

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
      Mesh *m = ProcessMesh(ento, model, mesh, scene);

      if (node->mNumMeshes > 1)
        m->_single_mesh = false;

      model._meshes->push_back(m);
      m->SetMaterialID(mesh->mMaterialIndex);
    }

    for (u32 i = 0; i < node->mNumChildren; i++) {
      Ento child;
      bool child_alreay_exists = false;

      for (Ento c : ento.Children()) {
        if (!c.HasComponent<Model>())
          continue;
        Model &m = c.GetComponent<Model>();
        if (m._mesh_id != i)
          continue;
        child = c;
        child_alreay_exists = true;
      }

      if (!child_alreay_exists) {
        child = Scene::GetActiveScene()->CreateEntity();
        child.AddComponent<Model>(model._path, model._shader_paths, false);
        ento.AddChild(child);
        log::debug("Created child model {}", node->mChildren[i]->mName.C_Str());

        log::debug("Parent had {} children", ento.Children().size());
      }

      Model &child_model = child.GetComponent<Model>();
      child_model.Init();
      child_model._mesh_id = i;

      if (!child_alreay_exists) {
        aiVector3D scale, position;
        aiQuaternion rotation;
        Transform &transform = child.GetComponent<Transform>();
        node->mTransformation.Decompose(scale, rotation, position);
        transform.SetPosition(v3(position.x, position.y, position.z));
        transform.SetRotation(quat(rotation.w, rotation.x, rotation.y, rotation.z));
        transform.SetScale(v3(scale.x, scale.y, scale.z));
      }

      log::debug("Processing node {}", node->mName.C_Str());
      ProcessNode(child, child_model, node->mChildren[i], scene);
    }
  }

  void Model::Draw() {
    for (Mesh *mesh : *_meshes) {
      Material &material = *(*_materials)[mesh->GetMaterialID()];
      material.BindAll();
      mesh->Draw();
    }
  }

  json Model::Serialize() const {
    json j;
    j["version"]["major"] = 0;
    j["version"]["minor"] = 1;
    j["type"] = "model";

    j["path"] = _path.string();
    j["mesh_id"] = _mesh_id;
    j["root"] = _root;
    return j;
  }

  void Model::Deserialize(const json &j) { }

  bool Model::ShowComponent() {
    bool modified = false;

    ImGui::Text("Path: %s", _path.string().c_str());

    return modified;
  }

} // namespace axl
