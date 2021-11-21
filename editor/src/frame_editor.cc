#include "frame_editor.h"

#include <imgui.h>
#include <axolotl/window.h>
#include <axolotl/iomanager.h>

#include <IconsFontAwesome5Pro.h>

namespace axl {

  FrameEditor::FrameEditor():
    bound_frame_ratio(false),
    _frame(1280, 720),
    _region_available({ 0, 0 })
  { }

  FrameEditor::~FrameEditor() { }

  void FrameEditor::Bind(Window &window) {
    _frame.Bind();
    window.GetRenderer()->Resize(_region_available.x, _region_available.y);
  }

  void FrameEditor::Unbind(Window &window) {
    _frame.Unbind();
    v2i size = window.GetFramebufferSize();
    window.GetRenderer()->Resize(size.x, size.y);
  }

  void FrameEditor::Draw(Window &window) {
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

    buffer_size.y -= 30;
    v2 texture_pos = (v2(ImGui::GetWindowSize()) - buffer_size) * 0.5f;
    texture_pos.y += 30;
    ImGui::SetCursorPos(texture_pos);
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

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 71) / 2.0f);
    ImGui::Button(ICON_FA_PLAY, v2(33, 24));
    ImGui::SameLine(0.0f, 5.0f);
    ImGui::Button(ICON_FA_PAUSE, v2(33, 24));
    ImGui::SameLine();

    // ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Hi there").x - 30);
    // ImGui::Text("Hi there");

    ImGui::End();
    ImGui::PopStyleVar();

    IOManager *io_manager = window.GetIOManager();
    if (!io_manager->ButtonDown(MouseButton::Left) && _region_available != current_region_available) {
      _region_available = current_region_available;
      _frame.SetSize(_region_available.x, _region_available.y);
      _frame.RebuildFrameBuffer();
    }
  }

  void FrameEditor::SetBoundFrameRatio(bool state) {
    bound_frame_ratio = state;
  }

} // namespace
