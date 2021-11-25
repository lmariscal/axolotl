#include "inspector.h"

#include <axolotl/scene.h>
#include <imgui.h>

namespace axl {

  void Inspector::Draw(Scene &scene) {
    ImGui::Begin("Inspector");
    entt::registry *registry = scene.GetRegistry();

    if (_selected_entity == entt::null) {
      ImGui::Text("No entity selected");
      ImGui::End();
      return;
    }

    Ento &ento = registry->get<Ento>(_selected_entity);

    std::string name = ento.name.empty() ? "Unnamed" : ento.name;
    name.resize(64);
    ImGui::Text("ID %s", uuids::to_string(ento.id).c_str());
    ImGui::Text("Name ");
    ImGui::SameLine();
    if (ImGui::InputText("##entity_name", name.data(), ento.name.capacity())) {
      ento.name = name;
    }

    for (Component *s : ento.components) {
      s->ShowData();
    }

    ImGui::End();
  }

} // namespace axl
