#include "inspector.h"

#include <axolotl/scene.h>
#include <axolotl/camera.h>
#include <axolotl/transform.h>
#include <axolotl/ento.h>
#include <axolotl/axolotl.h>
#include <axolotl/model.h>
#include <assimp/Importer.hpp>
#include <IconsFontAwesome5Pro.h>
#include <imgui.h>
#include <nfd.h>

namespace axl {

  Inspector::Inspector() {
  }

  void Inspector::Draw(Scene &scene) {
    ImGui::Begin("Inspector");
    if (!_selected_entity) {
      ImGui::End();
      return;
    }

    Ento ento = Scene::GetActiveScene()->FromID(_selected_entity);

    Tag &tag = ento.Tag();
    std::string name = tag.value.empty() ? "Unnamed" : tag.value;
    name.resize(64);
    ImGui::Text(ICON_FA_ID_BADGE " %s", uuids::to_string(ento.id).c_str());
    ImGui::Text("Name ");
    ImGui::SameLine();
    if (ImGui::InputText("##entity_name", name.data(), name.capacity())) {
      tag.value = name;
    }

    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_LAYER_PLUS " Add"))
      ImGui::OpenPopup("ComponentAddPopUp");

    ImGui::Separator();

    ShowComponents(_selected_entity, scene);

    ShowAddComponent(scene);

    if (_add_model) {
      ShaderPaths shader_paths = {
        Axolotl::GetDistDir() + "res/shaders/testy.vert",
        Axolotl::GetDistDir() + "res/shaders/testy.frag"
      };
      log::info("Adding model \"{}\"", _model_path);
      Model &model = _selected_entity.AddComponent<Model>(ento, _model_path, shader_paths);
      model.Init(ento);
      _add_model = false;
      _model_path = "";
    }

    if (_want_model)
      ImGui::OpenPopup("Load Model");
    if (ImGui::BeginPopupModal("Load Model", &_want_model, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Path:");

      if (ImGui::Button(ICON_FA_FOLDER " Open")) {
        nfdchar_t *out_path = nullptr;
        nfdresult_t result = NFD_OpenDialog("obj,gltf,fbx,glb", nullptr, &out_path);

        if (result == NFD_OKAY) {
          i32 len = std::strlen(out_path);
          _model_path.resize(len);
          std::strncpy(_model_path.data(), out_path, len);
          NFD_Free(out_path);
          ImGui::CloseCurrentPopup();
        } else if (result == NFD_CANCEL) {
          log::info("User pressed cancel");
        } else {
          log::error("Error: {}", NFD_GetError());
        }
      }
      ImGui::SameLine();

      std::array<char, 512> buffer;
      std::fill(buffer.begin(), buffer.end(), 0);
      std::strncpy(buffer.begin(), _model_path.c_str(), _model_path.size());
      if (ImGui::InputText("##model_path", buffer.data(), buffer.size()))
        _model_path = std::string(buffer.data());

      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_FILE_UPLOAD " Load")) {
        _add_model = true;
        _want_model = false;
        log::debug("Trying to load model\"{}\"", _model_path);
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    ImGui::End();
  }

  void Inspector::ShowAddComponent(Scene &scene) {
    if (ImGui::BeginPopup("ComponentAddPopUp")) {
      if (ImGui::MenuItem("Camera")) {
        _selected_entity.TryAddComponent<Camera>(_selected_entity);
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("Transform")) {
        _selected_entity.TryAddComponent<Transform>();
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("Model")) {
        log::debug("clearing model path");
        _want_model = true;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  void Inspector::ShowComponents(Ento &ento, Scene &scene) {
    if (ento.HasComponent<Transform>()) {
      if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        Transform &transform = ento.GetComponent<Transform>();
        if (transform.ShowComponent()) {

          if (_selected_entity.HasComponent<Camera>())
            _selected_entity.GetComponent<Camera>().UpdateVectors(_selected_entity);

        }
      }
    }

    if (ento.HasComponent<Camera>()) {
      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        Camera &camera = ento.GetComponent<Camera>();
        camera.ShowComponent();
      }
    }
  }

} // namespace axl
