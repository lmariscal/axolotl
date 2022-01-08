#include "menu.hh"

#include "level1.hh"
#include "level2.hh"

#include <axolotl/axolotl.hh>
#include <axolotl/camera.hh>
#include <axolotl/ento.hh>
#include <axolotl/renderer.hh>
#include <axolotl/window.hh>
#include <imgui.h>

namespace axl {

  void MenuLevel::Init(Window &window) {
    Ento camera_ento = CreateEntity();
    camera_ento.Tag().value = "Camera";
    Camera &camera_component = camera_ento.AddComponent<Camera>();
    camera_component.SetOrthographic();
    camera_component.SetAsActive();

    window.GetRenderer().SetShowGrid(false);
  }

  void MenuLevel::Update(Window &window, f64 delta) { }

  void MenuLevel::UpdateGUI(Window &window, const v2 &frame_size, const v2 &frame_pos) {
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(frame_pos.x + (frame_size.x * 0.1f), frame_pos.y + (frame_size.y * 0.1f)),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(frame_size.x * 0.6f, frame_size.y * 0.8f), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));

    ImGui::Begin("Menu", nullptr, window_flags);

    ImGuiIO &io = ImGui::GetIO();
    ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::Text("CSC8503 - Advanced Game Technologies");
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    if (ImGui::Button("Physics Level")) {
      Scene::SetActiveScene(new PhysicsLevel());
      log::debug("Physics Level");
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);

    if (ImGui::Button("AI Level")) {
      Scene::SetActiveScene(new AILevel());
      log::debug("AI Level");
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::End();
    ImGui::PopStyleVar();
  }

  void MenuLevel::Focused(Window &window, bool state) {
    focused = state;
  }

  void MenuLevel::ShowBackMenu(Window &window, const v2 &frame_size, const v2 &frame_pos, bool &show_menu) {
    IOManager &ioman = window.GetIOManager();

    if (ioman.KeyTriggered(Key::Escape))
      show_menu = !show_menu;

    Pad pad = Pad::Last;
    for (i32 p = 0; p < (i32)Pad::Last; ++p) {
      if (ioman.PadPresent((Pad)p)) {
        pad = (Pad)p;
        break;
      }
    }

    if (pad != Pad::Last) {
      if (ioman.ButtonTriggered(pad, PadButton::Start))
        show_menu = !show_menu;
    }

    if (!show_menu)
      return;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoDecoration;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(frame_pos.x + (frame_size.x * 0.1f), frame_pos.y + (frame_size.y * 0.1f)),
                            ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(frame_size.x * 0.6f, frame_size.y * 0.8f), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30, 30));

    ImGui::Begin("Menu", nullptr, window_flags);

    ImGuiIO &io = ImGui::GetIO();
    ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::Text("CSC8503 - Advanced Game Technologies");
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);

    if (ImGui::Button("Back to Main Menu")) {
      Scene::SetActiveScene(new MenuLevel());
      log::debug("AI Level");
    }
    if (ImGui::IsItemHovered())
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImGui::End();
    ImGui::PopStyleVar();
  }

} // namespace axl
