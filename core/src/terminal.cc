#include <axolotl/terminal.h>

#include <imterm/utils.hpp>
#include <imterm/misc.hpp>

#include <axolotl/shader.h>
#include <unordered_set>
#include <sstream>

namespace axl {

  constexpr std::array command_list {
    Terminal::command_type{ "clear", "Clear the terminal", Terminal::Clear, Terminal::NoCompletition },
    Terminal::command_type{ "echo", "Echo a message", Terminal::Echo, Terminal::NoCompletition },
    Terminal::command_type{ "shader", "Shader commands", Terminal::Shader, Terminal::NoCompletition },
    Terminal::command_type{ "quit", "Quit the editor", Terminal::Quit, Terminal::NoCompletition },
    Terminal::command_type{ "help", "Show commands information", Terminal::Help, Terminal::NoCompletition },
  };

  Terminal::Terminal() {
    for (auto &cmd : command_list)
      add_command_(cmd);
  }

  void Terminal::Init() {
    set_terminal_pattern("[%R:%S:%e] [user]: %v", ImTerm::message::type::user_input);
    set_terminal_pattern("[%R:%S:%e] [history]: %v", ImTerm::message::type::cmd_history_completion);
    set_terminal_pattern("[%R:%S:%e] [error]: %v", ImTerm::message::type::error);

    terminal_->theme() = ImTerm::themes::cherry;
    terminal_->set_autocomplete_pos(ImTerm::position::nowhere);
    log::default_logger()->sinks().push_back(terminal_->get_terminal_helper());
    log::set_pattern("[%R:%S:%e] [%^%l%$]: %v");
  }

  std::vector<std::string> Terminal::NoCompletition(argument_type &arg) {
    return { };
  }

  void Terminal::ShaderList(argument_type &arg) {
    if (arg.command_line.size() < 3) {
      for (const auto &s : Shader::_shaders_programs) {
        std::stringstream ss;
        ss << "  Shader program[" << s->_program << "]:";
        arg.term.add_text(ss.str());

        std::stringstream ss2;
        ss2 << "    ";
        for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
          if (s->_paths[i].empty())
            continue;
          std::string shader_type_name = Shader::ShaderTypeToString((ShaderType)i);
          if (i != 0)
            ss2 << ", ";
          ss2 << shader_type_name;
        }
        arg.term.add_text(ss2.str());
      }
      return;
    }

    std::string program = arg.command_line[2];
    bool program_found = false;
    for (const auto &s : Shader::_shaders_programs) {
      if (std::to_string(s->_program) != program)
        continue;
      program_found = true;
      std::stringstream ss;
      ss << "Shader program[" << program << "]:";
      arg.term.add_text(ss.str());

      for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
        if (s->_paths[i].empty())
          continue;
        std::stringstream ss2;
        std::string shader_type_name = Shader::ShaderTypeToString((ShaderType)i);
        ss2 << "  " << std::setw(8) << std::right << shader_type_name << ": " << s->_paths[i].string();
        arg.term.add_text(ss2.str());
      }
      return;
    }

    if (!program_found)
      arg.term.add_text_err("Shader program not found");
  }

  void Terminal::ShaderCompile(argument_type &arg) {
    if (arg.command_line.size() < 3) {
      arg.term.add_text("Usage: shader compile <program>\n");
      return;
    }

    std::string program = arg.command_line[2];
    bool program_found = false;
    for (const auto &s : Shader::_shaders_programs) {
      if (std::to_string(s->_program) != program)
        continue;
      program_found = true;
      s->Recompile();
      return;
    }

    if (!program_found)
      arg.term.add_text_err("Shader program not found");
  }

  void Terminal::ShaderReload(argument_type &arg) {
    if (arg.command_line.size() < 3) {
      arg.term.add_text("Usage: shader reload <program> [type]\n");
      return;
    }

    std::string program = arg.command_line[2];
    bool program_found = false;
    for (const auto &s : Shader::_shaders_programs) {
      if (std::to_string(s->_program) != program)
        continue;
      program_found = true;

      if (arg.command_line.size() < 4) {
        s->Reload();
        return;
      }

      std::string type = arg.command_line[3];
      ShaderType shader_type = Shader::StringToShaderType(type);
      if (shader_type == ShaderType::Last) {
        arg.term.add_text_err("Invalid shader type");
        return;
      }
      s->Reload(shader_type);
      return;
    }

    if (!program_found)
      arg.term.add_text_err("Shader program not found");
  }

  void Terminal::ShaderWatch(argument_type &arg) {
    if (arg.command_line.size() < 3) {
      std::stringstream ss;
      ss << "Watch shaders: " << (arg.val.watch_shaders ? "on" : "off");
      arg.term.add_text(ss.str());
      return;
    }

    std::string state = arg.command_line[2];
    if (state == "on") {
      arg.val.watch_shaders = true;
      arg.term.add_text("Shader watch is on");
    } else if (state == "off") {
      arg.val.watch_shaders = false;
      arg.term.add_text("Shader watch is off");
    } else {
      arg.term.add_text_err("Usage: shader watch <on/off>");
    }
  }

  void Terminal::Shader(argument_type &arg) {
    if (arg.command_line.size() < 2) {
      arg.term.add_text_err("Missing subcommand, try 'shader -h' for more information");
      return;
    }

    std::string subcommand = arg.command_line[1];
    if (subcommand == "compile") {
      ShaderCompile(arg);
    } else if (subcommand == "list") {
      ShaderList(arg);
    } else if (subcommand == "reload") {
      ShaderReload(arg);
    } else if (subcommand == "watch") {
      ShaderWatch(arg);
    } else if (subcommand == "--help" || subcommand == "-h") {
      arg.term.add_text("Shader commands:");
      arg.term.add_text("  compile <program>");
      arg.term.add_text("      Compile the shader program");
      arg.term.add_text("  list [program]");
      arg.term.add_text("      List all shader programs or the specified one");
      arg.term.add_text("  reload <program> [type]");
      arg.term.add_text("      Reload a shader program");
      arg.term.add_text("  watch <on/off>");
      arg.term.add_text("      Turn on/off shader watch mode");
    } else {
      std::stringstream ss;
      ss << "Unknown subcommand '" << subcommand << "', try 'shader -h' for more information";
      arg.term.add_text_err(ss.str());
    }
  }

  void Terminal::Echo(argument_type &arg) {
    if (arg.command_line.size() < 2)
     return;

    std::stringstream ss;
    for (i32 i = 1; i < arg.command_line.size(); ++i) {
      ss << arg.command_line[i];
      if (i != arg.command_line.size() - 1)
        ss << " ";
    }

    arg.term.add_text(ss.str());
  }

  void Terminal::Clear(argument_type &arg) {
    arg.term.clear();
  }

  void Terminal::Quit(argument_type &arg) {
    arg.val.quit_requested = true;
  }

  void Terminal::Help(argument_type &arg) {
    constexpr u64 list_element_name_max_size =
      misc::max_size(command_list.begin(), command_list.end(),
        [](const command_type& cmd) {
          return cmd.name.size();
        }
      );

    arg.term.add_text("Available commands:");
    for (const command_type& cmd : command_list) {
      std::stringstream ss;
      ss << std::setw(list_element_name_max_size) << std::right << cmd.name << " | " << cmd.description;
      arg.term.add_text(ss.str());
    }
    arg.term.add_text("Additional information might be available using \"'command' --help\" or \"'command' -h\"");
  }
} // namespace axl
