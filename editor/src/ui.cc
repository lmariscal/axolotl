#include "ui.h"

#include <imgui.h>
#include <axolotl/window.h>
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

    buffer_size.y -= 30;
    v2 texture_pos = (v2(ImGui::GetWindowSize()) - buffer_size) * 0.5f;
    texture_pos.y += 30;
    ImGui::SetCursorPos(texture_pos);

    v2 cursor_pos = ImGui::GetCursorPos();
    if (data.scene_playing && ImGui::InvisibleButton("##click_frame", buffer_size))
      frame_focused = true;
    ImGui::SetCursorPos(cursor_pos);
    ImGui::Image((ImTextureID)(size_t)_frame.GetTextureID(FrameBufferTexture::Color), buffer_size, uv0, uv1);

    ImGui::SetCursorPosX(5);
    ImGui::SetCursorPosY(25);
    ImGui::Button(ICON_FA_EXPAND_ARROWS, v2(24, 24));
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::Button(ICON_FA_UNDO, v2(24, 24));
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::Button(ICON_FA_EXPAND_ALT, v2(24, 24));
    ImGui::SameLine();

    ImGui::Checkbox(" Frame Ratio", &bound_frame_ratio);
    ImGui::SameLine();
    ImGui::Checkbox(" Fullscreen", &fullscreen_play);
    ImGui::SameLine();

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

    // ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Hi there").x - 30);
    // ImGui::Text("Hi there");

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

  void FrameEditor::ShowTreeEnto(Ento *ento, u32 depth) {
    ImGui::PushID(uuids::to_string(ento->id).c_str());
    std::stringstream label;
    label << ICON_FA_BOX << " ";
    label << (ento->name.empty() ? uuids::to_string(ento->id) : ento->name);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
    if (ento->children.empty()) {
      flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 18.0f);
    bool clicked = ImGui::TreeNodeEx(label.str().c_str(), flags);
    if (ImGui::IsItemClicked())
      _inspector._selected_entity = *ento;
    ImGui::PopStyleVar();

    ImGui::PopID();

    if (!clicked || ento->children.empty())
      return;

    for (Ento *c : ento->children)
      ShowTreeEnto(c, depth + 1);
    ImGui::TreePop();
    return;
  }

  void FrameEditor::DrawEntityList(Scene &scene) {
    ImGui::ShowDemoWindow();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, v2(0.0f, 12.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, v2(0.0f, 6.0f));
    ImGui::Begin("Entities");
    entt::registry *registry = scene.GetRegistry();
    auto view = registry->view<Ento>();
    for (auto entity : view) {
      Ento &ento = registry->get<Ento>(entity);
      if (ento.parent)
        continue;
      ShowTreeEnto(&ento, 0);
    }
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
