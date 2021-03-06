#pragma once

#include <axolotl/types.hh>
#include <imterm/terminal.hpp>
#include <imterm/terminal_helpers.hpp>
#include <mutex>

namespace axl {

  class TerminalData {
   public:
    bool quit_requested = false;
    bool watch_shaders = true;
    bool scene_playing = true;
    bool scene_paused = false;
    bool display_terminal = false;

    v2 frame_size = { 0, 0 };
    v2 frame_pos = { 0, 0 };
    std::filesystem::path project_path = "";
  };

  class Terminal: public ImTerm::basic_spdlog_terminal_helper<Terminal, TerminalData, std::mutex> {
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
    void Terminate();
  };

} // namespace axl
