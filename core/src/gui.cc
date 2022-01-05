// clang-format off
#include <GLFW/glfw3.h>
#include <IconsFontAwesome5Pro.h>
#include <axolotl/axolotl.hh>
#include <axolotl/gui.hh>
#include <axolotl/window.hh>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
// clang-format on

namespace axl {

  void FontAddFontAwesomeChars(const std::string &font_awesome_path, ImGuiIO &io, f32 size) {
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    ImFont *font = io.Fonts->AddFontFromFileTTF(font_awesome_path.c_str(), size, &icons_config, icons_ranges);
    if (!font) {
      log::error("Failed to load font \"{}\"", font_awesome_path);
    }
  }

  void ImGuiFonts() {
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();
    std::string path = Axolotl::GetDistDir() + "res/misc/JetBrainsMono-Regular.ttf";
    ImFont *font = io.Fonts->AddFontFromFileTTF(path.c_str(), 20.0f);
    if (!font) {
      io.Fonts->AddFontDefault();
      log::error("Failed to load GUI font\n...");
      return;
    }

    std::string font_awesome_path = Axolotl::GetDistDir() + "res/misc/Font Awesome 6 Pro-Solid-900.otf";
    FontAddFontAwesomeChars(font_awesome_path, io, 16.0f);

    io.Fonts->Build();

    std::string bold_path = Axolotl::GetDistDir() + "res/misc/JetBrainsMono-Bold.ttf";
    ImFont *bold_font = io.Fonts->AddFontFromFileTTF(bold_path.c_str(), 20.0f);
    if (!bold_font) {
      log::error("Failed to load Bold GUI font\n...");
      return;
    }

    FontAddFontAwesomeChars(font_awesome_path, io, 16.0f);
    io.Fonts->Build();

    io.FontGlobalScale = 0.8f;
  }

  void YADarkTheme() {
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    style.ColorButtonPosition = ImGuiDir_Left;
    style.ScrollbarSize = 12.0f;
    style.IndentSpacing = 19.0f;
    style.WindowRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
    style.PopupBorderSize = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
  }

  void CherryTheme() {
#define HI(v)  v4(0.502f, 0.075f, 0.256f, v)
#define MED(v) v4(0.455f, 0.198f, 0.301f, v)
#define LOW(v) v4(0.232f, 0.201f, 0.271f, v)
// backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v) v4(0.200f, 0.220f, 0.270f, v)
// text
#define TEXT(v) v4(0.860f, 0.930f, 0.890f, v)

    auto &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = TEXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled] = TEXT(0.28f);
    style.Colors[ImGuiCol_WindowBg] = v4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
    style.Colors[ImGuiCol_Border] = v4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow] = v4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
    style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
    style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = v4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
    style.Colors[ImGuiCol_CheckMark] = v4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = v4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive] = v4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button] = v4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
    style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
    style.Colors[ImGuiCol_Header] = MED(0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
    style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = v4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
    style.Colors[ImGuiCol_PlotLines] = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);

    style.Colors[ImGuiCol_Tab] = MED(0.43f);
    style.Colors[ImGuiCol_TabHovered] = MED(0.73f);
    style.Colors[ImGuiCol_TabUnfocused] = MED(0.23f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = HI(0.23f);
    style.Colors[ImGuiCol_TabActive] = HI(0.43f);

    style.WindowPadding = v2(6, 4);
    style.WindowRounding = 0.0f;
    style.FramePadding = v2(5, 2);
    style.FrameRounding = 3.0f;
    style.ItemSpacing = v2(7, 1);
    style.ItemInnerSpacing = v2(1, 1);
    style.TouchExtraPadding = v2(0, 0);
    style.IndentSpacing = 6.0f;
    style.ScrollbarSize = 12.0f;
    style.ScrollbarRounding = 16.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = v4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
  }

  GUI::GUI(Window *window): _lock_mouse(false) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigDockingWithShift = true;

    // CherryTheme();
    YADarkTheme();
    ImGuiFonts();

    ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWWindow(), false);
    ImGui_ImplOpenGL3_Init();

    io.ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable;
  }

  GUI::~GUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();
  }

  void GUI::Update() {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
  }

  void GUI::Draw() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow *current_window = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(current_window);
    }
  }

  void GUI::WindowFocusCallback(GLFWwindow *window, i32 focused) {
    ImGui_ImplGlfw_WindowFocusCallback(window, focused);
  }

  void GUI::CursorEnterCallback(GLFWwindow *window, i32 entered) {
    ImGui_ImplGlfw_CursorEnterCallback(window, entered);
  }

  void GUI::MouseButtonCallback(GLFWwindow *window, i32 button, i32 action, i32 mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
  }

  void GUI::ScrollCallback(GLFWwindow *window, f64 x_offset, f64 y_offset) {
    ImGui_ImplGlfw_ScrollCallback(window, x_offset, y_offset);
  }

  void GUI::KeyCallback(GLFWwindow *window, i32 key, i32 scancode, i32 action, i32 mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
  }

  void GUI::CharCallback(GLFWwindow *window, u32 character) {
    ImGui_ImplGlfw_CharCallback(window, character);
  }

  void GUI::MonitorCallback(GLFWmonitor *monitor, i32 event) {
    ImGui_ImplGlfw_MonitorCallback(monitor, event);
  }

  void GUI::LockMouse(bool state) {
    _lock_mouse = state;

    ImGuiIO &io = ImGui::GetIO();

    if (state) {
      io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
      io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    } else {
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
      io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    }
    // Remove if statement via binary operations and state

    // io.ConfigFlags = (io.ConfigFlags & ~ImGuiConfigFlags_NoMouse) | (state ? ImGuiConfigFlags_NoMouse : 0);
    // io.ConfigFlags = (io.ConfigFlags & ~ImGuiConfigFlags_NoMouseCursorChange) | (state ?
    // ImGuiConfigFlags_NoMouseCursorChange : 0); io.ConfigFlags = (io.ConfigFlags &
    // ~ImGuiConfigFlags_NavEnableKeyboard) | (state ? ImGuiConfigFlags_NavEnableKeyboard : 0); io.ConfigFlags =

    io.WantCaptureKeyboard = state;
    io.WantCaptureMouse = state;
  }

} // namespace axl
