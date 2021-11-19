#include <axolotl/shader.h>

#include <fstream>
#include <sstream>
#include <glad.h>

namespace axl {

  std::string ShaderTypeToString(ShaderType type) {
    switch (type) {
      case ShaderType::Vertex:
        return "Vertex";
      case ShaderType::Fragment:
        return "Fragment";
      case ShaderType::Geometry:
        return "Geometry";
      case ShaderType::Compute:
        return "Compute";
      default:
        return "Unknown";
    }
  }

  u32 ShaderTypeToGL(ShaderType type) {
    switch (type) {
      case ShaderType::Vertex:
        return GL_VERTEX_SHADER;
      case ShaderType::Fragment:
        return GL_FRAGMENT_SHADER;
      case ShaderType::Geometry:
        return GL_GEOMETRY_SHADER;
      case ShaderType::Compute:
        return GL_COMPUTE_SHADER;
      default:
        return 0;
    }
  }

  Shader::Shader():
    _program(0)
  {
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      _shaders[i] = 0;
      _paths[i] = "";
    }
  }

  Shader::~Shader() {
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (_shaders[i] == 0)
        continue;

      glDeleteShader(_shaders[i]);
    }

    glDeleteProgram(_program);
  }

  void Shader::Load(ShaderType type, const std::filesystem::path &path) {
    log::debug("Loading shader, type {}, path \"{}\"", ShaderTypeToString(type), path.string());
    std::string data = Read(path);

    _paths[(i32)type] = path;
    LoadData(type, data);
  }

  void Shader::LoadData(ShaderType type, const std::string &data) {
    _shaders[(i32)type] = CompileShader(type, data);
  }

  std::string Shader::Read(const std::filesystem::path &path) {
    if (!std::filesystem::exists(path)) {
      log::error("Shader file \"{}\" does not exist", path.string());
      return "";
    }

    std::ifstream file(path);
    std::stringstream buffer;
    std::string line;

    while (std::getline(file, line)) {
      if (line.empty())
        continue;

      if (line.find("#include ") == 0) {
        buffer << "// " << line << " START\n";
        std::string include = line.substr(9);
        if (include.rfind(".vert") != 5 && include.rfind(".frag") != 5 && include.rfind(".glsl") != 5) {
          if (include.rfind('.') != std::string::npos) {
            log::error("Shader include \"{}\" does not have a valid extension", include);
            buffer << "// include " << include << " does not have a valid extension\n";
            buffer << "// " << line << " END\n";
            continue;
          }
          include += ".glsl";
        }

        std::filesystem::path include_path = path.parent_path() / include;

        // 9 is the length of "#include ", magic number
        log::debug("Found include directive, path \"{}\", resolved \"{}\"", include, include_path.string());

        std::string include_data = Read(include_path);
        if (include_data.empty()) {
          log::error("Shader include \"{}\" could not be read", include_path.string());
          buffer << "// include " << include << " could not be read\n";
          buffer << "// " << line << " END\n";
          continue;
        }

        buffer << include_data;
        buffer << "// " << line << " END\n";
        continue;
      }

      buffer << line << '\n';
    }

    return buffer.str();
  }

  bool Shader::Compile() {
    if (_program) {
      log::error("Shader program {} already compiled", _program);
      return false;
    }

    _program = glCreateProgram();

    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (_shaders[i] == 0)
        continue;

      glAttachShader(_program, _shaders[i]);
      log::debug("Attached shader {}, to program {}", _shaders[i], _program);
    }

    glLinkProgram(_program);

    i32 status;
    glGetProgramiv(_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
      i32 length;
      glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);

      std::string log(length, ' ');
      glGetProgramInfoLog(_program, length, &length, &log[0]);

      log::error("Shader linking failed\n{}", log);
      return false;
    }

    log::debug("Shader program {} compiled successfully", _program);

    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (_shaders[i] == 0)
        continue;

      glDetachShader(_program, _shaders[i]);
    }

    return true;
  }

  u32 Shader::CompileShader(ShaderType type, const std::string &data) {
    if (data.empty())
      return 0;

    u32 shader_id = glCreateShader(ShaderTypeToGL(type));

    const char *cstr = data.c_str();
    glShaderSource(shader_id, 1, &cstr, nullptr);
    glCompileShader(shader_id);

    i32 status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
      i32 length;
      glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

      std::string log(length, ' ');
      glGetShaderInfoLog(shader_id, length, &length, &log[0]);

      log::error("Shader {} compilation failed, type: {}, path \"{}\"\n{}", shader_id, ShaderTypeToString(type), _paths[(i32)type].string(), log);
      log::error("Shader data\n{}", data);

      glDeleteShader(shader_id);
      return 0;
    }

    log::debug("Shader {} compiled, type {}", shader_id, ShaderTypeToString(type));
    return shader_id;
  }

  bool Shader::Recompile() {
    if (_program)
      glDeleteProgram(_program);
    _program = 0;
    return Compile();
  }

  void Shader::Bind() {
    glUseProgram(_program);
  }

  void Shader::Unload(ShaderType type) {
    if (_shaders[(i32)type] == 0)
      return;

    glDeleteShader(_shaders[(i32)type]);
    _shaders[(i32)type] = 0;
  }

  bool Shader::Reload(ShaderType type) {
    if (type == ShaderType::Last) {
      for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
        if (_paths[i].empty())
          continue;
        if (!Reload((ShaderType)i))
          return false;
      }
      return true;
    }

    if (_paths[(i32)type].empty())
      return false;

    log::debug("Reloading shader program {}, type {}", _program, ShaderTypeToString(type));

    std::string data = Read(_paths[(i32)type]);
    if (data.empty())
      return false;

    u32 shader_id = CompileShader(type, data);
    if (shader_id == 0)
      return false;

    if (_shaders[(i32)type] != 0)
      glDeleteShader(_shaders[(i32)type]);
    _shaders[(i32)type] = shader_id;

    log::debug("Shader program {}, type {} reloaded successfully", _program, ShaderTypeToString(type));
    return true;
  }

} // namespace axl
