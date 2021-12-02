#include "ui.hh"

#include <imgui.h>
#include <axolotl/window.hh>
#include <axolotl/transform.hh>
#include <axolotl/camera.hh>
#include <axolotl/iomanager.hh>
#include <axolotl/ento.hh>
#include <axolotl/scene.hh>
#include <axolotl/terminal.hh>

#include "dockspace.hh"

#include <IconsFontAwesome5Pro.h>

namespace axl {

  FrameEditor::FrameEditor():
    _frame(1280, 720),
    _region_available({ 0, 0 }),
    action(EditorAction::Select)
  { }

  FrameEditor::~FrameEditor() { }

  void FrameEditor::Bind(Window &window) {
    _frame.Bind();
  }

  void FrameEditor::Unbind(Window &window) {
    _frame.Unbind();
    v2i size = window.GetWindowFrameBufferSize();
    window.GetRenderer().Resize(size.x, size.y);
  }

  void FrameEditor::Draw(Window &window, DockSpace &dock) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 0.0f));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::Begin("World Editor", &dock.data.show_world_editor, window_flags);

    v2 uv0(0.0f, 1.0f);
    v2 uv1(1.0f, 0.0f);
    v2 cra = ImGui::GetContentRegionAvail();
    v2 buffer_size = cra;
    if (dock.data.ratio_locked) {
      if (cra.x * 9.0f / 16.0f > cra.y) {
        buffer_size.x = cra.y * 16.0f / 9.0f;
        buffer_size.y = cra.y;
      } else {
        buffer_size.y = cra.x * 9.0f / 16.0f ;
        buffer_size.x = cra.x;
      }
    }
    v2 current_region_available = v2(buffer_size);

    IOManager &io = window.GetIOManager();

    v2 cursor_pos = ImGui::GetCursorPos();

    buffer_size.y -= 30 + (cursor_pos.y * 0.1f);
    v2 texture_pos = (v2(ImGui::GetWindowSize()) - buffer_size) * 0.5f;
    texture_pos.y += 30 + (cursor_pos.y * 0.1f);
    ImGui::SetCursorPos(texture_pos);

    bool one_camera_active = Camera::GetActiveCamera() != nullptr;

    if (one_camera_active) {
      ImGui::Image((ImTextureID)(size_t)_frame.GetTexture(FrameBufferTexture::Color), buffer_size, uv0, uv1);
      if (dock.data.terminal->scene_playing && ImGui::IsItemClicked())
        focused = true;
    }

    ImGui::SetCursorPos(v2(5, (cursor_pos.y * 0.66f) + 12));
    // ImGui::Checkbox(" Frame Ratio", &bound_frame_ratio);
    // ImGui::SameLine();

    ImVec4 *colors = ImGui::GetStyle().Colors;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 71) / 2.0f);

    bool playing = dock.data.terminal->scene_playing;
    if (playing) {
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    } else {
      ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, v2(0.36, 0.50));
    }
    if (ImGui::Button(dock.data.terminal->scene_playing ? ICON_FA_STOP : ICON_FA_PLAY, v2(33, 24))) {
      dock.data.terminal->scene_playing = !dock.data.terminal->scene_playing;
      if (dock.data.terminal->scene_playing)
        focused = true;
      if (!dock.data.terminal->scene_playing && dock.data.terminal->scene_paused)
        dock.data.terminal->scene_paused = false;
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Play/Stop Scene");
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    if (playing) {
      ImGui::PopStyleColor();
    } else {
      ImGui::PopStyleVar();
    }

    ImGui::SameLine(0.0f, 5.0f);
    bool paused = dock.data.terminal->scene_paused;
    if (paused)
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    if (ImGui::Button(ICON_FA_PAUSE, v2(33, 24)))
      dock.data.terminal->scene_paused = !dock.data.terminal->scene_paused;
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Pause Scene");
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
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

      EditorAction original_action = action;
      ImGui::SetWindowFontScale(1.1f);

      if (original_action == EditorAction::Select)
        ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
      ImGui::SetCursorPos(cursor_pos);
      if (ImGui::Button(ICON_FA_HAND_POINTER, v2(30, 30)))
        action = EditorAction::Select;
      if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      cursor_pos.x += 30;
      if (original_action == EditorAction::Select)
        ImGui::PopStyleColor();

      if (original_action == EditorAction::Move)
        ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
      ImGui::SetCursorPos(cursor_pos);
      if (ImGui::Button(ICON_FA_ARROWS, v2(30, 30)))
        action = EditorAction::Move;
      if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      cursor_pos.x += 30;
      if (original_action == EditorAction::Move)
        ImGui::PopStyleColor();

      if (original_action == EditorAction::Rotate)
        ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
      ImGui::SetCursorPos(cursor_pos);
      if (ImGui::Button(ICON_FA_SYNC_ALT, v2(30, 30)))
        action = EditorAction::Rotate;
      if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      cursor_pos.x += 30;
      if (original_action == EditorAction::Rotate)
        ImGui::PopStyleColor();

      if (original_action == EditorAction::Scale)
        ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
      ImGui::SetCursorPos(cursor_pos);
      if (ImGui::Button(ICON_FA_EXPAND, v2(30, 30)))
        action = EditorAction::Scale;
      if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      cursor_pos.x += 30;
      if (original_action == EditorAction::Scale)
        ImGui::PopStyleColor();

      ImGui::SetWindowFontScale(1.0f);
      ImGui::PopStyleColor(3);
    }

    if (!one_camera_active) {
      v2 window_size = ImGui::GetWindowSize();
      std::string text = "No active camera";
      v2 text_size = ImGui::CalcTextSize(text.c_str());
      ImGui::SetCursorPos(v2(window_size.x - text_size.x, window_size.y) / 2.0f);
      ImGui::TextColored(v4(v3(1.0f), 0.6f), "%s", text.c_str());
    }

    ImGui::End();
    ImGui::PopStyleVar();

    if (!io.ButtonDown(MouseButton::Left) && _region_available != current_region_available) {
      _region_available = current_region_available;
      _frame.SetSize(_region_available.x, _region_available.y);
      _frame.RebuildFrameBuffer();
    }
  }

  void FrameEditor::ShowTreeEnto(Ento ento, u32 depth, Scene &scene) {
    ImGui::PushID(uuids::to_string(ento.id).c_str());
    std::stringstream label;
    label << ICON_FA_CIRCLE_NOTCH << " ";
    label << (ento.Tag().value.empty() ? uuids::to_string(ento.id) : ento.Tag().value);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;
    if (!ento.HasChildren())
      flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (_inspector._selected_entity == ento)
      flags |= ImGuiTreeNodeFlags_Selected;

    // ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 18.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, v2(4.0f, 4.0f));
    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (depth * 18.0f));
    bool tree_open = ImGui::TreeNodeEx(label.str().c_str(), flags);
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    if (ImGui::IsItemClicked())
      _inspector._selected_entity = ento;
    ImGui::PopStyleVar(1);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 0.0f));
    bool marked_for_deletion = false;
    if (ImGui::BeginPopupContextItem()) {
      marked_for_deletion = ShowEntityPopUp(ento, scene);
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::PopID();

    if (tree_open && ento.HasChildren()) {
      for (Ento child : ento.Children()) {
        ShowTreeEnto(child, depth + 1, scene);
      }

      ImGui::TreePop();
    }

    if (marked_for_deletion)
      scene.RemoveEntity(ento);

    return;
  }

  bool FrameEditor::ShowEntityPopUp(Ento ento, Scene &scene) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, v2(0.0f, 0.0f));
    ImGui::SetWindowFontScale(0.95f);

    f32 width = ImGui::GetWindowWidth();
    v2 button_size = v2(100.0f, ImGui::GetTextLineHeight() + 9.0f);
    if (ImGui::Button(ICON_FA_PLUS_CIRCLE " Add", button_size)) {
      scene.CreateEntity();
      ImGui::CloseCurrentPopup();
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    if (!ento) {
      ImGui::PopStyleVar();
      return false;
    }

    bool marked_for_deletion = false;
    if (ImGui::Button(ICON_FA_TRASH_ALT " Delete", button_size)) {
      marked_for_deletion = true;
      _inspector._selected_entity = { };
      ImGui::CloseCurrentPopup();
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::PopStyleVar();
    return marked_for_deletion;
  }

  void FrameEditor::DrawEntityList(Scene &scene, DockSpace &dock) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 9.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2(0.0f, 0.0f));
    ImGui::Begin("Entities", &dock.data.show_hierarchy);

    f32 width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 40.0f;
    ImGui::SetCursorPosX(20.0f);

    std::array<char, 256> buffer;
    std::fill(buffer.begin(), buffer.end(), 0);
    std::strcpy(buffer.data(), _search_string.c_str());

    ImGui::PushItemWidth(width);
    if (ImGui::InputTextWithHint("##search", ICON_FA_SEARCH " Search", buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
      _search_string = buffer.data();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9.0f);

    ImGui::Separator();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);

    scene._registry.each([&](auto entity) {
      Ento ento = scene.FromHandle(entity);
      if (!ento || ento.HasParent())
        return;
      ShowTreeEnto(ento, 0, scene);
    });

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
      _inspector._selected_entity = { };

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 0.0f));
    if (ImGui::BeginPopupContextWindow("entity_popup", 1, false)) {
      ShowEntityPopUp({ }, scene);
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::End();
    ImGui::PopStyleVar(2);
  }

  void FrameEditor::DrawInspector(Scene &scene, DockSpace &dock) {
    _inspector.Draw(scene, dock);
  }

  const v2 & FrameEditor::GetRegionAvailable() const {
    return _region_available;
  }

} // namespace
