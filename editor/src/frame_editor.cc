#include "frame_editor.h"

#include <imgui.h>
#include <axolotl/window.h>
#include <axolotl/iomanager.h>

namespace axl {

  FrameEditor::FrameEditor():
    _frame(1280, 720)
  {

  }

  FrameEditor::~FrameEditor() {
  }

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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("World Editor");
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;

    ImVec2 uv0 = ImVec2(0.0f, 1.0f);
    ImVec2 uv1 = ImVec2(1.0f, 0.0f);
    ImVec2 cra = ImGui::GetContentRegionAvail();
    ImVec2 buffer_size = cra;
    if (_bound_frame_ratio) {
      if (cra.x * 9.0f / 16.0f > cra.y) {
        buffer_size.x = cra.y * 16.0f / 9.0f;
        buffer_size.y = cra.y;
      } else {
        buffer_size.y = cra.x * 9.0f / 16.0f ;
        buffer_size.x = cra.x;
      }
    }
    v2 current_region_available = v2(buffer_size.x, buffer_size.y);

    ImGui::Image((ImTextureID)(size_t)_frame.GetTextureID(FrameBufferTexture::Color), buffer_size, uv0, uv1);

    ImGui::End();
    ImGui::PopStyleVar();

    IOManager *io_manager = window.GetIOManager();
    if (_region_available != current_region_available && !io_manager->ButtonDown(MouseButton::Left)) {
      _region_available = current_region_available;
      _frame.SetSize(_region_available.x, _region_available.y);
      _frame.RebuildFrameBuffer();
    }
  }

  void FrameEditor::SetBoundFrameRatio(bool state) {
    _bound_frame_ratio = state;
  }

} // namespace
