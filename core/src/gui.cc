#include <axolotl/gui.h>

#include <axolotl/axolotl.h>
#include <axolotl/window.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

namespace axl {

  void CherryTheme() {
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();
    std::string path = Axolotl::GetDistDir() + "res/misc/JetBrainsMono-Regular.ttf";
    log::debug("Font path {}", path);
    ImFont *font = io.Fonts->AddFontFromFileTTF(path.c_str(), 16.0f);
    if (!font) {
      io.Fonts->AddFontDefault();
      log::error("Failed to load GUI font\n...");
    }
    io.Fonts->Build();

    #define HI(v)   v4(0.502f, 0.075f, 0.256f, v)
    #define MED(v)  v4(0.455f, 0.198f, 0.301f, v)
    #define LOW(v)  v4(0.232f, 0.201f, 0.271f, v)
    // backgrounds (@todo: complete with BG_MED, BG_LOW)
    #define BG(v)   v4(0.200f, 0.220f, 0.270f, v)
    // text
    #define TEXT(v) v4(0.860f, 0.930f, 0.890f, v)

    auto &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text]                  = TEXT(0.78f);
    style.Colors[ImGuiCol_TextDisabled]          = TEXT(0.28f);
    style.Colors[ImGuiCol_WindowBg]              = v4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]               = BG( 0.9f);
    style.Colors[ImGuiCol_Border]                = v4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]          = v4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]               = BG( 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]        = MED( 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]         = MED( 1.00f);
    style.Colors[ImGuiCol_TitleBg]               = LOW( 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]         = HI( 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]      = BG( 0.75f);
    style.Colors[ImGuiCol_MenuBarBg]             = BG( 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]           = BG( 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]         = v4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = MED( 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = MED( 1.00f);
    style.Colors[ImGuiCol_CheckMark]             = v4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]            = v4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]      = v4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                = v4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_ButtonActive]          = MED( 1.00f);
    style.Colors[ImGuiCol_Header]                = MED( 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]         = MED( 0.86f);
    style.Colors[ImGuiCol_HeaderActive]          = HI( 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]            = v4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]     = MED( 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotLines]             = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]      = MED( 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]         = TEXT(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered]  = MED( 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]        = MED( 0.43f);

    style.WindowPadding            = v2(6, 4);
    style.WindowRounding           = 0.0f;
    style.FramePadding             = v2(5, 2);
    style.FrameRounding            = 3.0f;
    style.ItemSpacing              = v2(7, 1);
    style.ItemInnerSpacing         = v2(1, 1);
    style.TouchExtraPadding        = v2(0, 0);
    style.IndentSpacing            = 6.0f;
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;
    style.GrabMinSize              = 20.0f;
    style.GrabRounding             = 2.0f;

    style.WindowTitleAlign.x = 0.50f;

    style.Colors[ImGuiCol_Border] = v4(0.539f, 0.479f, 0.255f, 0.162f);
    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
  }

  GUI::GUI(Renderer *renderer, Window *window):
    _renderer(renderer)
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigDockingWithShift = true;

    CherryTheme();

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

  void GUI::WindowFocusCallback(GLFWwindow* window, i32 focused) {
    ImGui_ImplGlfw_WindowFocusCallback(window, focused);
  }

  void GUI::CursorEnterCallback(GLFWwindow* window, i32 entered) {
    ImGui_ImplGlfw_CursorEnterCallback(window, entered);
  }

  void GUI::MouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
  }

  void GUI::ScrollCallback(GLFWwindow* window, f64 x_offset, f64 y_offset) {
    ImGui_ImplGlfw_ScrollCallback(window, x_offset, y_offset);
  }

  void GUI::KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
  }

  void GUI::CharCallback(GLFWwindow* window, u32 character) {
    ImGui_ImplGlfw_CharCallback(window, character);
  }

  void GUI::MonitorCallback(GLFWmonitor* monitor, i32 event) {
    ImGui_ImplGlfw_MonitorCallback(monitor, event);
  }


} // namespace axl
