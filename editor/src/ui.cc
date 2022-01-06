#include "ui.hh"

#include "dockspace.hh"

#include <IconsFontAwesome5Pro.h>
#include <ImGuizmo.h>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/iomanager.hh>
#include <axolotl/scene.hh>
#include <axolotl/terminal.hh>
#include <axolotl/transform.hh>
#include <axolotl/window.hh>
#include <entt/entt.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>

namespace axl {

  FrameEditor::FrameEditor():
    _frame(1280, 720),
    _region_available({ 0, 0 }),
    action(EditorAction::Select),
    focused(false) { }

  FrameEditor::~FrameEditor() { }

  bool FrameEditor::CompareEntoID(const Ento &a, const Ento &b) {
    return a.id < b.id;
  }

  void FrameEditor::DrawActionButtons(Window &window, DockSpaceData &dock_space_data) {
    ImVec4 *colors = ImGui::GetStyle().Colors;

    v4 button_color = colors[(i32)ImGuiCol_Button];
    v4 button_color_active = colors[(i32)ImGuiCol_ButtonActive];
    v4 button_color_hovered = colors[(i32)ImGuiCol_ButtonHovered];
    button_color.w = 0.75f;
    button_color_active.w = 0.75f;
    button_color_hovered.w = 0.75f;
    ImGui::PushStyleColor(ImGuiCol_Button, button_color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, button_color_active);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, button_color_hovered);

    v2 cursor_pos = v2(12, 12);

    EditorAction original_action = action;
    ImGui::SetWindowFontScale(1.1f);

    if (original_action == EditorAction::Select)
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    ImGui::SetCursorPos(cursor_pos);
    if (ImGui::Button(ICON_FA_HAND_POINTER, v2(30, 30)))
      action = EditorAction::Select;
    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      dock_space_data.hover_action_buttons = true;
    }
    cursor_pos.x += 30;
    if (original_action == EditorAction::Select)
      ImGui::PopStyleColor();

    if (original_action == EditorAction::Move)
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    ImGui::SetCursorPos(cursor_pos);
    if (ImGui::Button(ICON_FA_ARROWS, v2(30, 30)))
      action = EditorAction::Move;
    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      dock_space_data.hover_action_buttons = true;
    }
    cursor_pos.x += 30;
    if (original_action == EditorAction::Move)
      ImGui::PopStyleColor();

    if (original_action == EditorAction::Rotate)
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    ImGui::SetCursorPos(cursor_pos);
    if (ImGui::Button(ICON_FA_SYNC_ALT, v2(30, 30)))
      action = EditorAction::Rotate;
    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      dock_space_data.hover_action_buttons = true;
    }
    cursor_pos.x += 30;
    if (original_action == EditorAction::Rotate)
      ImGui::PopStyleColor();

    if (original_action == EditorAction::Scale)
      ImGui::PushStyleColor(ImGuiCol_Button, colors[(i32)ImGuiCol_ButtonActive]);
    ImGui::SetCursorPos(cursor_pos);
    if (ImGui::Button(ICON_FA_EXPAND, v2(30, 30)))
      action = EditorAction::Scale;
    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      dock_space_data.hover_action_buttons = true;
    }
    cursor_pos.x += 30;
    if (original_action == EditorAction::Scale)
      ImGui::PopStyleColor();

    if (!focused) {
      IOManager &io = window.GetIOManager();
      if (io.KeyTriggered(Key::Q))
        action = EditorAction::Select;
      else if (io.KeyTriggered(Key::W))
        action = EditorAction::Move;
      else if (io.KeyTriggered(Key::E))
        action = EditorAction::Rotate;
      else if (io.KeyTriggered(Key::R))
        action = EditorAction::Scale;
    }

    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopStyleColor(3);
  }

  const std::set<Ento, decltype(FrameEditor::CompareEntoID) *>
  FrameEditor::FilterEntities(Scene &scene, const std::vector<Ento> &ents, bool root) {
    std::set<Ento, decltype(CompareEntoID) *> entities(CompareEntoID);

    for (Ento ento : ents) {
      if (!ento) {
        log::error("Entity is null");
        continue;
      }

      bool search = !_search_string.empty();
      if (search) {
        if (ento.Tag().value.find(_search_string) == std::string::npos &&
            uuids::to_string(ento.id).find(_search_string) == std::string::npos)
          continue;
      }

      entities.insert(ento);

      if (!search)
        continue;

      auto parent = ento.Parent();
      while (parent) {
        entities.insert(parent);
        parent = parent.Parent();
      }
    }

    return entities;
  }

  void FrameEditor::DrawGuizmo(Window &window, Camera *camera) {
    if (!camera)
      return;

    v2 window_pos = ImGui::GetWindowPos();
    v2 window_size = ImGui::GetWindowSize();

    m4 view = camera->GetViewMatrix(nullptr);
    m4 proj = camera->GetProjectionMatrix(window);

    v2 view_manipulate_pos = window_pos;
    view_manipulate_pos.x += window_size.x - 128 - 12;
    view_manipulate_pos.y += 12;
    ImGuizmo::ViewManipulate(value_ptr(view), 8.0f, view_manipulate_pos, v2(128), 0x10101010);

    Ento selected_entity = Scene::GetActiveScene()->FromID(_inspector._selected_entity_id);
    if (!selected_entity)
      return;
    if (action == EditorAction::Select)
      return;

    ImGuizmo::SetRect(window_pos.x, window_pos.y, window_size.x, window_size.y);

    IOManager &io = window.GetIOManager();

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();

    ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
    if (action == EditorAction::Rotate)
      operation = ImGuizmo::ROTATE;
    else if (action == EditorAction::Scale)
      operation = ImGuizmo::SCALE;

    Transform &transform = selected_entity.Transform();
    m4 model = transform.GetModelMatrix();

    v3 snap(action == EditorAction::Rotate ? 45.0f : 0.5f);
    bool snap_enabled = io.KeyDown(Key::LeftControl) || io.KeyDown(Key::RightControl);
    bool bounds_enabled = io.KeyDown(Key::LeftShift) || io.KeyDown(Key::RightShift);

    std::array<f32, 6> bounds = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
    std::array<f32, 6> bounds_snap;
    std::fill(bounds_snap.begin(), bounds_snap.end(), snap.x);

    ImGuizmo::Manipulate(value_ptr(view),
                         value_ptr(proj),
                         operation,
                         ImGuizmo::LOCAL,
                         value_ptr(model),
                         nullptr,
                         snap_enabled ? value_ptr(snap) : nullptr,
                         bounds_enabled ? &bounds[0] : nullptr,
                         snap_enabled ? value_ptr(snap) : nullptr);

    if (!ImGuizmo::IsUsing())
      return;

    v3 skew;
    v4 perspective;

    quat rotation;
    v3 scale;
    v3 translation;

    decompose(model, scale, rotation, translation, skew, perspective);

    if (!std::isnan(rotation.x) && !std::isnan(rotation.y) && !std::isnan(rotation.z) && !std::isnan(rotation.w))
      transform.SetRotation(rotation);
    else
      transform.SetRotation(quat());
    if (!std::isnan(translation.x) && !std::isnan(translation.y) && !std::isnan(translation.z))
      transform.SetPosition(translation);
    else
      transform.SetPosition(v3());
    if (!std::isnan(scale.x) && !std::isnan(scale.y) && !std::isnan(scale.z))
      transform.SetScale(scale);
    else
      transform.SetScale(v3(1.0f));
  }

  void FrameEditor::Bind(Window &window) {
    _frame.Bind();
  }

  void FrameEditor::Unbind(Window &window) {
    _frame.Unbind();
    v2i size = window.GetWindowFrameBufferSize();
    window.GetRenderer().Resize(size.x, size.y);
  }

  void FrameEditor::Draw(Window &window, DockSpace &dock, Camera *camera) {
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
        buffer_size.y = cra.x * 9.0f / 16.0f;
        buffer_size.x = cra.x;
      }
    }
    v2 current_region_available = v2(buffer_size);

    IOManager &io = window.GetIOManager();

    v2 cursor_pos = ImGui::GetCursorPos();

    buffer_size.y -= 30 + (cursor_pos.y * 0.1f);
    _region_cursor = (v2(ImGui::GetWindowSize()) - buffer_size) * 0.5f;
    _region_cursor.y += 30 + (cursor_pos.y * 0.1f);
    ImGui::SetCursorPos(_region_cursor);

    bool one_camera_active = Camera::GetActiveCamera() != nullptr;

    if (one_camera_active) {
      dock.data.hover_frame_editor = false;
      ImGui::BeginChild("##frame_editor_region", buffer_size, true, ImGuiWindowFlags_NoScrollbar);
      ImGui::Image((ImTextureID)(size_t)_frame.GetTexture(FrameBufferTexture::Color), buffer_size, uv0, uv1);
      if (ImGui::IsItemHovered())
        dock.data.hover_frame_editor = true;
      if (!focused && dock.data.terminal->scene_playing && !dock.data.terminal->scene_paused &&
          ImGui::IsItemClicked() && !ImGuizmo::IsOver() && !dock.data.hover_action_buttons)
        focused = true;

      if (!dock.data.terminal->scene_playing || dock.data.terminal->scene_paused) {
        DrawGuizmo(window, camera);

        dock.data.hover_action_buttons = false;
        DrawActionButtons(window, dock.data);
      }

      dock.data.terminal->frame_pos = ImGui::GetWindowPos();
      dock.data.terminal->frame_size = ImGui::GetWindowSize();
      ImGui::EndChild();
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
      if (dock.data.terminal->scene_playing) {
        focused = true;
        _pre_play_state = dock.data.scene->Serialize();
      } else {
        if (!_pre_play_state.empty())
          dock.data.scene->Deserialize(_pre_play_state);
      }
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

  void FrameEditor::ShowTreeEnto(Ento ento,
                                 u32 depth,
                                 Scene &scene,
                                 const std::set<Ento, decltype(CompareEntoID) *> &entities) {
    Ento selected_entity = Scene::GetActiveScene()->FromID(_inspector._selected_entity_id);

    ImGui::PushID(uuids::to_string(ento.id).c_str());
    std::stringstream label;
    label << ICON_FA_CIRCLE_NOTCH << " ";
    label << (ento.Tag().value.empty() ? uuids::to_string(ento.id) : ento.Tag().value);

    ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_FramePadding;

    if (!ento.HasChildren())
      flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    if (selected_entity == ento)
      flags |= ImGuiTreeNodeFlags_Selected;

    // ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 18.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, v2(4.0f, 4.0f));
    // ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (depth * 18.0f));
    bool tree_open = ImGui::TreeNodeEx(label.str().c_str(), flags);
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    if (ImGui::IsItemClicked())
      _inspector._selected_entity_id = ento.id;
    ImGui::PopStyleVar(1);

    bool marked_for_deletion = false;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(8.0f, 8.0f));
    if (ImGui::BeginPopupContextItem()) {
      marked_for_deletion = ShowEntityPopUp(ento, scene);
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::PopID();

    if (tree_open && ento.HasChildren()) {
      for (const Ento &child : ento.Children()) {
        if (!entities.count(child))
          continue;
        ShowTreeEnto(child, depth + 1, scene, entities);
      }

      ImGui::TreePop();
    }

    if (marked_for_deletion)
      scene.RemoveEntity(ento);

    return;
  }

  bool FrameEditor::ShowEntityPopUp(Ento ento, Scene &scene) {
    if (ImGui::MenuItem(ICON_FA_PLUS_CIRCLE " Add")) {
      scene.CreateEntity();
      ImGui::CloseCurrentPopup();
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    if (!ento)
      return false;

    bool marked_for_deletion = false;
    if (ImGui::MenuItem(ICON_FA_TRASH_ALT " Delete")) {
      marked_for_deletion = true;
      _inspector._selected_entity_id = {};
      ImGui::CloseCurrentPopup();
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    return marked_for_deletion;
  }

  void FrameEditor::DrawEntityList(Scene &scene, DockSpace &dock) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 8.0f));
    ImGui::Begin("Entities", &dock.data.show_hierarchy);

    f32 width = ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - 40.0f;
    ImGui::SetCursorPosX(20.0f);

    std::array<char, 256> buffer;
    std::fill(buffer.begin(), buffer.end(), 0);
    std::strcpy(buffer.data(), _search_string.c_str());

    ImGui::PushItemWidth(width);
    if (ImGui::InputTextWithHint("##search", ICON_FA_SEARCH " Search", buffer.data(), buffer.size()))
      _search_string = buffer.data();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9.0f);

    ImGui::Separator();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);

    std::vector<Ento> entities;
    scene._registry.each([&](auto entity) { entities.push_back(scene.FromHandle(entity)); });
    auto filtered_entities = FilterEntities(scene, entities, true);

    for (const Ento &ento : filtered_entities) {
      if (!scene._registry.valid(ento.handle))
        continue;
      if (ento.HasParent())
        continue;
      ShowTreeEnto(ento, 0, scene, filtered_entities);
    }

    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
      _inspector._selected_entity_id = {};

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(8.0f, 8.0f));
    if (ImGui::BeginPopupContextWindow("entity_popup",
                                       ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
      ShowEntityPopUp({}, scene);
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void FrameEditor::DrawInspector(Scene &scene, DockSpace &dock) {
    _inspector.Draw(scene, dock);
  }

  const v2 &FrameEditor::GetRegionAvailable() const {
    return _region_available;
  }

} // namespace axl
