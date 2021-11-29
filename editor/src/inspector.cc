#include "inspector.h"

#include <axolotl/scene.h>
#include <axolotl/camera.h>
#include <axolotl/transform.h>
#include <axolotl/axolotl.h>
#include <axolotl/model.h>
#include <IconsFontAwesome5Pro.h>
#include <imgui.h>
#include <nfd.h>

namespace axl {

  Inspector::Inspector() {
  }

  void Inspector::Draw(Scene &scene) {
    ImGui::Begin("Inspector");
    entt::registry *registry = scene.GetRegistry();

    if (_selected_entity == entt::null) {
      ImGui::End();
      return;
    }

    Ento &ento = registry->get<Ento>(_selected_entity);

    std::string name = ento.name.empty() ? "Unnamed" : ento.name;
    name.resize(64);
    ImGui::Text(ICON_FA_ID_BADGE " %s", uuids::to_string(ento.id).c_str());
    ImGui::Text("Name ");
    ImGui::SameLine();
    if (ImGui::InputText("##entity_name", name.data(), name.capacity())) {
      ento.name = name;
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_LAYER_PLUS " Add"))
      ImGui::OpenPopup("ComponentAddPopUp");

    ImGui::Separator();

    for (Component *s : ento.components) {
      s->ShowData();
    }

    ShowAddComponent(scene);

    if (_add_model) {
      ShaderPaths shader_paths = {
        Axolotl::GetDistDir() + "res/shaders/testy.vert",
        Axolotl::GetDistDir() + "res/shaders/testy.frag"
      };
      log::info("Adding model \"{}\"", _model_path);
      scene.TryAddComponent<Model>(_selected_entity, _model_path, shader_paths);
      _add_model = false;
      _model_path = "";
    }

    if (_want_model)
      ImGui::OpenPopup("ModelPopUp");
    if (ImGui::BeginPopupModal("ModelPopUp", &_want_model, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Model Path");

      if (ImGui::Button(ICON_FA_FOLDER " Open")) {
        nfdchar_t *out_path = nullptr;
        nfdresult_t result = NFD_OpenDialog("gltf", nullptr, &out_path);

        if (result == NFD_OKAY) {
          i32 len = std::strlen(out_path);
          _model_path.resize(len);
          std::strncpy(_model_path.data(), out_path, len);
          NFD_Free(out_path);

          _add_model = true;
          _want_model = false;
          log::info("Selected model path: {}", _model_path);
          ImGui::CloseCurrentPopup();
        } else if (result == NFD_CANCEL) {
          log::info("User pressed cancel");
        } else {
          log::error("Error: {}", NFD_GetError());
        }
      }
      ImGui::SameLine();

      std::array<char, 256> buffer;
      std::fill(buffer.begin(), buffer.end(), 0);
      std::strncpy(buffer.begin(), _model_path.c_str(), _model_path.size());
      if (ImGui::InputText("##model_path", buffer.data(), 256))
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
        scene.TryAddComponent<Camera>(_selected_entity);
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("Transform")) {
        scene.TryAddComponent<Transform>(_selected_entity);
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

} // namespace axl
