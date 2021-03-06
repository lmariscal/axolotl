#include "inspector.hh"

#include <IconsFontAwesome5Pro.h>
#include <assimp/Importer.hpp>
#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/geometry.hh>
#include <axolotl/light.hh>
#include <axolotl/model.hh>
#include <axolotl/physics.hh>
#include <axolotl/scene.hh>
#include <axolotl/transform.hh>
#include <imgui.h>
#include <nfd.h>

namespace axl {

  Inspector::Inspector() { }

  void Inspector::Draw(Scene &scene, DockSpace &dock) {
    ImGui::Begin("Inspector", &dock.data.show_inspector);
    Ento selected_entity = scene.FromID(_selected_entity_id);
    if (!selected_entity) {
      v2 size = ImGui::GetWindowSize();
      std::string text = "No entity selected";
      f32 width = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosY(size.y / 2.0f);
      ImGui::SetCursorPosX((size.x - width) / 2.0f);
      ImGui::TextColored(v4(v3(1.0f), 0.6f), "%s", text.c_str());

      ImGui::End();
      return;
    }

    v2 id_text_size = ImGui::CalcTextSize(uuids::to_string(selected_entity.id).c_str());
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - id_text_size.x - ImGui::GetStyle().FramePadding.x) / 2.0f);

    ImGui::TextColored(v4(1.0f, 1.0f, 1.0f, 0.6f), "%s", uuids::to_string(selected_entity.id).c_str());
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    if (ImGui::IsItemClicked())
      ImGui::SetClipboardText(uuids::to_string(selected_entity.id).c_str());

    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().ItemSpacing.y);

    if (_edit_name) {
      Tag &tag = selected_entity.Tag();

      std::array<char, 128> name_buffer;
      std::fill(name_buffer.begin(), name_buffer.end(), 0);
      std::copy(tag.value.begin(), tag.value.end(), name_buffer.begin());
      if (ImGui::InputText("##entity_name",
                           name_buffer.data(),
                           name_buffer.size(),
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        tag.value = name_buffer.data();
      }
      if (_edit_name_first) {
        ImGui::SetKeyboardFocusHere(-1);
        _edit_name_first = false;
      } else if (!ImGui::IsItemActive()) {
        tag.value = name_buffer.data();
        _edit_name = false;
        _edit_name_first = true;
      }
    } else {
      ImGui::SetWindowFontScale(1.1f);
      ImGui::Text("%s", selected_entity.Tag().value.c_str());
      if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
      if (ImGui::IsItemClicked()) {
        _edit_name = true;
        _edit_name_first = true;
      }
      ImGui::SetWindowFontScale(1.0f);
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - 60.0f);
    if (ImGui::Button(ICON_FA_LAYER_PLUS " Add", v2(60, 0)))
      ImGui::OpenPopup("ComponentAddPopUp");
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::Separator();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1.0f);

    ShowComponents(selected_entity, scene);

    ShowAddComponent(scene);

    if (_add_model) {
      std::vector<std::string> shader_paths = { Axolotl::GetDistDir() + "res/shaders/testy.vert",
                                                Axolotl::GetDistDir() + "res/shaders/testy.frag" };
      log::info("Adding model \"{}\"", _model_path);
      Model &model = selected_entity.AddComponent<Model>(_model_path, shader_paths);
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
          ImGui::CloseCurrentPopup();

          NFD_Free(out_path);
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
    Ento selected_entity = scene.FromID(_selected_entity_id);
    if (!selected_entity)
      return;

    if (ImGui::BeginPopup("ComponentAddPopUp")) {
      if (ImGui::MenuItem("Camera")) {
        Camera &camera = selected_entity.TryAddComponent<Camera>();
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("Transform")) {
        selected_entity.TryAddComponent<Transform>();
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("Model")) {
        log::debug("clearing model path");
        _want_model = true;
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("Light")) {
        selected_entity.TryAddComponent<Light>(LightType::Point, v3(1.0f), 0.6f);
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("RigidBody")) {
        selected_entity.TryAddComponent<RigidBody>();
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("OBB Collider")) {
        selected_entity.TryAddComponent<OBBCollider>();
        ImGui::CloseCurrentPopup();
      }
      if (ImGui::MenuItem("Sphere Collider")) {
        selected_entity.TryAddComponent<SphereCollider>();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
  }

  void Inspector::ShowComponents(Ento &ento, Scene &scene) {
    Ento selected_entity = scene.FromID(_selected_entity_id);
    if (!selected_entity)
      return;

    if (ento.HasComponent<Transform>()) {
      if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        Transform &transform = ento.GetComponent<Transform>();
        if (transform.ShowComponent()) {

          if (selected_entity.HasComponent<Camera>())
            selected_entity.GetComponent<Camera>().UpdateVectors(&selected_entity.Transform());
        }
      }
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
    }

    if (ento.HasComponent<Camera>()) {
      if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        Camera &camera = ento.GetComponent<Camera>();
        camera.ShowComponent(ento);
      }
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
    }

    if (ento.HasComponent<Model>()) {
      if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen)) {
        Model &model = ento.GetComponent<Model>();
        model.ShowComponent();
      }
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
    }

    if (ento.HasComponent<Light>()) {
      if (ImGui::CollapsingHeader("Light Source", ImGuiTreeNodeFlags_DefaultOpen)) {
        Light &light = ento.GetComponent<Light>();
        light.ShowComponent();
      }
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
    }

    if (ento.HasComponent<RigidBody>()) {
      if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) {
        RigidBody &rigid_body = ento.GetComponent<RigidBody>();
        rigid_body.ShowComponent();
      }
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
    }

    if (ento.HasComponent<OBBCollider>()) {
      if (ImGui::CollapsingHeader("OBB Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
        OBBCollider &collider = ento.GetComponent<OBBCollider>();
        collider.ShowComponent();
      }
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
    }

    if (ento.HasComponent<SphereCollider>()) {
      if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
        SphereCollider &collider = ento.GetComponent<SphereCollider>();
        collider.ShowComponent();
      }
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);
    }
  }

} // namespace axl
