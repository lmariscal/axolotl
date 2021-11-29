#include "ui.h"

#include <imgui.h>
#include <axolotl/window.h>
#include <axolotl/transform.h>
#include <axolotl/camera.h>
#include <axolotl/iomanager.h>
#include <axolotl/ento.h>
#include <axolotl/scene.h>
#include <axolotl/terminal.h>

#include <IconsFontAwesome5Pro.h>

namespace axl {

  FrameEditor::FrameEditor():
    bound_frame_ratio(false),
    frame_focused(false),
    fullscreen_play(false),
    _frame(1280, 720),
    _region_available({ 0, 0 })
  { }

  FrameEditor::~FrameEditor() { }

  void FrameEditor::Bind(Window &window) {
    _frame.Bind();
  }

  void FrameEditor::Unbind(Window &window) {
    _frame.Unbind();
    v2i size = window.GetWindowFrameBufferSize();
    window.GetRenderer()->Resize(size.x, size.y);
  }

  void FrameEditor::Draw(Window &window, TerminalData &data) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 0.0f));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin("World Editor", nullptr, window_flags);

    v2 uv0(0.0f, 1.0f);
    v2 uv1(1.0f, 0.0f);
    v2 cra = ImGui::GetContentRegionAvail();
    v2 buffer_size = cra;
    if (bound_frame_ratio) {
      if (cra.x * 9.0f / 16.0f > cra.y) {
        buffer_size.x = cra.y * 16.0f / 9.0f;
        buffer_size.y = cra.y;
      } else {
        buffer_size.y = cra.x * 9.0f / 16.0f ;
        buffer_size.x = cra.x;
      }
    }
    v2 current_region_available = v2(buffer_size);

    IOManager *io_manager = window.GetIOManager();

    v2 cursor_pos = ImGui::GetCursorPos();

    buffer_size.y -= 30 + (cursor_pos.y * 0.1f);
    v2 texture_pos = (v2(ImGui::GetWindowSize()) - buffer_size) * 0.5f;
    texture_pos.y += 30 + (cursor_pos.y * 0.1f);
    ImGui::SetCursorPos(texture_pos);

    bool one_camera_active = Camera::GetActiveCamera() != nullptr;

    if (one_camera_active) {
      ImGui::Image((ImTextureID)(size_t)_frame.GetTextureID(FrameBufferTexture::Color), buffer_size, uv0, uv1);
      if (data.scene_playing && ImGui::IsItemClicked())
        frame_focused = true;
    }

    ImGui::SetCursorPos(v2(5, (cursor_pos.y * 0.66f) + 12));
    // ImGui::Checkbox(" Frame Ratio", &bound_frame_ratio);
    // ImGui::SameLine();

    ImVec4 *colors = ImGui::GetStyle().Colors;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 71) / 2.0f);

    bool playing = data.scene_playing;
    if (playing) {
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    } else {
      ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, v2(0.36, 0.50));
    }
    if (ImGui::Button(data.scene_playing ? ICON_FA_STOP : ICON_FA_PLAY, v2(33, 24))) {
      data.scene_playing = !data.scene_playing;
      if (data.scene_playing)
        frame_focused = true;
      if (!data.scene_playing && data.scene_paused)
        data.scene_paused = false;
    }
    if (playing) {
      ImGui::PopStyleColor();
    } else {
      ImGui::PopStyleVar();
    }

    ImGui::SameLine(0.0f, 5.0f);
    bool paused = data.scene_paused;
    if (paused)
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    if (ImGui::Button(ICON_FA_PAUSE, v2(33, 24)))
      data.scene_paused = !data.scene_paused;
    if (paused)
      ImGui::PopStyleColor();
    ImGui::SameLine();

    if (one_camera_active) {
      // ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Hi there").x - 30);
      // ImGui::Text("Hi there");

      v4 button_color = colors[(i32)ImGuiCol_Button];
      v4 button_color_active = colors[(i32)ImGuiCol_ButtonActive];
      v4 button_color_hovered = colors[(i32)ImGuiCol_ButtonHovered];
      button_color.w = 0.75f;
      button_color_active.w = 0.75f;
      button_color_hovered.w = 0.75f;
      ImGui::PushStyleColor(ImGuiCol_Button, button_color);
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_color_active);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_color_hovered);

      cursor_pos = ImGui::GetCursorPos();
      cursor_pos.x = 12;
      cursor_pos.y += 42;
      ImGui::SetCursorPos(cursor_pos);
      ImGui::Button(ICON_FA_HAND_POINTER, v2(30, 30));
      cursor_pos.x += 30;
      ImGui::SetCursorPos(cursor_pos);
      ImGui::Button(ICON_FA_ARROWS, v2(30, 30));
      cursor_pos.x += 30;
      ImGui::SetCursorPos(cursor_pos);
      ImGui::Button(ICON_FA_SYNC_ALT, v2(30, 30));
      cursor_pos.x += 30;
      ImGui::SetCursorPos(cursor_pos);
      ImGui::Button(ICON_FA_EXPAND, v2(30, 30));

      ImGui::PopStyleColor(3);
    }

    if (!one_camera_active) {
      v2 window_size = ImGui::GetWindowSize();
      std::string text = "No active camera";
      v2 text_size = ImGui::CalcTextSize(text.c_str());
      ImGui::SetCursorPos(v2(window_size.x - text_size.x, window_size.y) / 2.0f);
      ImGui::Text("%s", text.c_str());
    }

    ImGui::End();
    ImGui::PopStyleVar();

    if (!io_manager->ButtonDown(MouseButton::Left) && _region_available != current_region_available) {
      _region_available = current_region_available;
      _frame.SetSize(_region_available.x, _region_available.y);
      _frame.RebuildFrameBuffer();
    }
  }

  void FrameEditor::SetBoundFrameRatio(bool state) {
    bound_frame_ratio = state;
  }

  void FrameEditor::ShowTreeEnto(Ento *ento, u32 depth, Scene &scene) {
    ImGui::PushID(uuids::to_string(ento->id).c_str());
    std::stringstream label;
    label << ICON_FA_BOX << " ";
    label << (ento->name.empty() ? uuids::to_string(ento->id) : ento->name);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
    if (ento->children.empty())
      flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (_inspector._selected_entity == *ento)
      flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 18.0f);
    bool tree_open = ImGui::TreeNodeEx(label.str().c_str(), flags);
    if (ImGui::IsItemClicked())
      _inspector._selected_entity = *ento;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(9.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2(9.0f, 6.0f));
    if (ImGui::BeginPopupContextItem()) {
      ShowEntityPopUp(ento, scene);
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2);

    ImGui::PopStyleVar();
    ImGui::PopID();

    if (!tree_open || ento->children.empty())
      return;

    for (Ento *c : ento->children)
      ShowTreeEnto(c, depth + 1, scene);
    ImGui::TreePop();
    return;
  }

  void FrameEditor::ShowEntityPopUp(Ento *ento, Scene &scene) {
    if (ImGui::MenuItem(ICON_FA_PLUS_CIRCLE " Add")) {
      entt::entity new_entity = scene.CreateEntity();
      scene.AddComponent<Transform>(new_entity);
      ImGui::CloseCurrentPopup();
    }

    if (!ento)
      return;

    if (ImGui::MenuItem(ICON_FA_TRASH_ALT " Delete")) {
      ento->marked_for_deletion = true;
      _inspector._selected_entity = entt::null;
      ImGui::CloseCurrentPopup();
    }
  }

  void FrameEditor::DrawEntityList(Scene &scene) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2(0.0f, 6.0f));
    ImGui::Begin("Entities");

    entt::registry *registry = scene.GetRegistry();
    auto view = registry->view<Ento>();
    for (auto entity : view) {
      Ento &ento = registry->get<Ento>(entity);
      if (ento.parent)
        continue;
      ShowTreeEnto(&ento, 0, scene);
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
      _inspector._selected_entity = entt::null;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(9.0f, 6.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2(9.0f, 6.0f));
    if (ImGui::BeginPopupContextWindow("entity_popup", 1, false)) {
      ShowEntityPopUp(nullptr, scene);
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar(2);

    ImGui::End();
    ImGui::PopStyleVar(2);
  }

  void FrameEditor::DrawInspector(Scene &scene) {
    _inspector.Draw(scene);
  }

  const v2 & FrameEditor::GetRegionAvailable() const {
    return _region_available;
  }

} // namespace
