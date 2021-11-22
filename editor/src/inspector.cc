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
    ImGui::Text("ID %s", uuids::to_string(ento.id).c_str());
    ImGui::Text("Name ");
    ImGui::SameLine();
    ImGui::InputText("##entity_name", ento.name.data(), ento.name.capacity());

    for (Serializable *s : ento.components) {
      s->ShowDataToUI();
    }

    ImGui::End();
  }

} // namespace axl
