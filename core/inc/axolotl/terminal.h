#pragma once

#include <axolotl/types.h>
#include <imterm/terminal.hpp>
#include <imterm/terminal_helpers.hpp>
#include <mutex>

namespace axl {

  struct TerminalData {
    bool quit_requested = false;
    bool watch_shaders = true;
    bool scene_playing = false;
    bool scene_paused = false;
    bool show_imgui_demo = false;
  };

  class Terminal : public ImTerm::basic_spdlog_terminal_helper<Terminal, TerminalData, std::mutex> {
   public:
    static std::vector<std::string> NoCompletition(argument_type &arg);

    static void Clear(argument_type &arg);
    static void Echo(argument_type &arg);
    static void Quit(argument_type &arg);
    static void Help(argument_type &arg);

    static void Shader(argument_type &arg);
    static void ShaderList(argument_type &arg);
    static void ShaderReload(argument_type &arg);
    static void ShaderCompile(argument_type &arg);
    static void ShaderWatch(argument_type &arg);

    Terminal();

    void Init();
  };

} // namespace axl
