#include <axolotl/shader.h>

#include <fstream>
#include <sstream>
#include <glad.h>
#include <efsw/efsw.hpp>
#include <iostream>

namespace axl {

  Uniform::Uniform(UniformType type, UniformDataType data_type, std::string name, ShaderType shader_type, json json):
    type(type),
    data_type(data_type),
    name(name),
    shader_type(shader_type),
    value(json)
  { }

  Uniform::Uniform():
    type(UniformType::Last),
    data_type(UniformDataType::Last),
    name(""),
    shader_type(ShaderType::Last),
    value(json::object())
  { }

  class ShaderWatcher : public efsw::FileWatchListener {
   public:
    ShaderWatcher(Shader* shader): _shader(shader) { }
    virtual ~ShaderWatcher() { }

    virtual void handleFileAction(efsw::WatchID watchid, const std::string &dir, const std::string &filename,
                                  efsw::Action action, std::string old_name = "") {
      if (action != efsw::Actions::Modified)
        return;

      ShaderType type = ShaderType::Last;
      for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
        if (_shader->_paths[i].empty())
          continue;
        if (filename != _shader->_paths[i].filename().string())
          continue;
        if (dir != _shader->_paths[i].parent_path().string() + "/")
          continue;
        type = (ShaderType)i;
        break;
      }

      if (type == ShaderType::Last)
        return;

      _shader->_need_reload[(i32)type] = true;
    }

   private:
    Shader *_shader;
  };

  std::string Shader::ShaderTypeToString(ShaderType type) {
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

  ShaderType Shader::StringToShaderType(const std::string &str) {
    if (str == "Vertex")
      return ShaderType::Vertex;
    if (str == "Fragment")
      return ShaderType::Fragment;
    if (str == "Geometry")
      return ShaderType::Geometry;
    if (str == "Compute")
      return ShaderType::Compute;
    return ShaderType::Last;
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

  UniformType StringToUniformType(const std::string &str) {
    if (str == "model")
      return UniformType::Model;
    if (str == "view")
      return UniformType::View;
    if (str == "projection")
      return UniformType::Projection;
    if (str == "time")
      return UniformType::Time;
    if (str == "resolution")
      return UniformType::Resolution;
    if (str == "mouse")
      return UniformType::Mouse;
    if (str == "mouse_delta")
      return UniformType::MouseDelta;
    if (str == "other")
      return UniformType::Other;
    if (str.find("texture") == 0)
      return UniformType::Texture;
    return UniformType::Last;
  }

  UniformDataType StringToUniformDataType(const std::string &str) {
    if (str == "float")
      return UniformDataType::Float;
    if (str == "int")
      return UniformDataType::Int;
    if (str == "vec2")
      return UniformDataType::Vec2;
    if (str == "vec3")
      return UniformDataType::Vec3;
    if (str == "vec4")
      return UniformDataType::Vec4;
    if (str == "mat4")
      return UniformDataType::Mat4;
    if (str == "sampler2D")
      return UniformDataType::Sampler2D;
    if (str == "color")
      return UniformDataType::Color;
    return UniformDataType::Last;
  }


  void Shader::Init() {
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      _shaders[i] = 0;
      _watch_ids[i] = 0;
      _need_reload[i] = false;
    }
    _uniforms[(i32)UniformType::Texture].resize(MaxTextures);
    for (i32 i = 0; i < MaxTextures; ++i) {
      _uniforms[(i32)UniformType::Texture][i].type = UniformType::Texture;
      _uniforms[(i32)UniformType::Texture][i].data_type = UniformDataType::Sampler2D;
      _uniforms[(i32)UniformType::Texture][i].name = "";
      _uniform_texture_cache[i] = 0;
    }

    _watcher = new efsw::FileWatcher();
    _shader_watcher = new ShaderWatcher(this);

    _watcher->watch();

    _shaders_programs.push_back(this);

    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (_paths[i].empty())
        continue;
      Load((ShaderType)i, _paths[i]);
    }
  }

  Shader::Shader(const ShaderPaths &paths):
    _program(0)
  {
    if (!paths.vertex.empty())
      _paths[(i32)ShaderType::Vertex] = paths.vertex;
    if (!paths.fragment.empty())
      _paths[(i32)ShaderType::Fragment] = paths.fragment;
    if (!paths.geometry.empty())
      _paths[(i32)ShaderType::Geometry] = paths.geometry;
    if (!paths.compute.empty())
      _paths[(i32)ShaderType::Compute] = paths.compute;
  }

  Shader::Shader():
    _program(0)
  {
  }

  Shader::~Shader() {
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (_shaders[i] == 0)
        continue;

      glDeleteShader(_shaders[i]);

      if (_watch_ids[i] == 0 || _paths[i].empty())
        continue;

      _watcher->removeWatch(_watch_ids[i]);
    }

    glDeleteProgram(_program);

    delete _shader_watcher;
    delete _watcher;

    _shaders_programs.erase(std::find(_shaders_programs.begin(), _shaders_programs.end(), this));
  }

  bool Shader::Watch() {
    bool need_recompile = false;
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (!_need_reload[i])
        continue;

      if (Reload((ShaderType)i))
        need_recompile = true;

      _need_reload[i] = false;
    }
    return need_recompile;
  }

  void Shader::Load(ShaderType type, const std::filesystem::path &path) {
    if (_shaders[(i32)type] != 0) {
      log::error("Shader already loaded, type {}, path \"{}\"", ShaderTypeToString(type), _paths[(i32)type].string());
      return;
    }

    log::debug("Loading shader, type {}, path \"{}\"", ShaderTypeToString(type), path.string());
    std::string data = Read(path, type);

    _paths[(i32)type] = path;
    LoadData(type, data);

    efsw::WatchID watch_id = _watcher->addWatch(path.parent_path().string(), _shader_watcher, false);
    _watch_ids[(i32)type] = watch_id;
  }

  void Shader::LoadData(ShaderType type, const std::string &data) {
    if (_shaders[(i32)type] != 0) {
      log::error("Shader already loaded, type {}, path \"{}\"", ShaderTypeToString(type), _paths[(i32)type].string());
      return;
    }

    _shaders[(i32)type] = CompileShader(type, data);
  }

  void Shader::ParseUniform(std::string line, ShaderType shader_type) {
    std::string line_type;
    line_type = line.substr(0, line.find(' '));
    line = line.substr(line_type.length() + 1);

    std::string line_data_type;
    line_data_type = line.substr(0, line.find(' '));
    line = line.substr(line_data_type.length() + 1);

    std::string name = line;

    UniformType type = StringToUniformType(line_type);
    UniformDataType data_type = StringToUniformDataType(line_data_type);

    if (type == UniformType::Last || data_type == UniformDataType::Last) {
      log::error("Unknown uniform type \"{}\"", line);
      return;
    }

    json j;
    switch (data_type) {
      case UniformDataType::Float: {
        j["value"] = "0.0f";
        break;
      }
      case UniformDataType::Int: {
        j["value"] = 0;
        break;
      }
      case UniformDataType::Vec2: {
        j["value"] = v2(0.0f);
        break;
      }
      case UniformDataType::Vec3: {
        j["value"] = v3(0.0f);
        break;
      }
      case UniformDataType::Vec4: {
        j["value"] = v4(0.0f);
        break;
      }
      case UniformDataType::Mat4: {
        // j["value"] = m4(0.0f);
        break;
      }
      case UniformDataType::Sampler2D: {
        j["value"] = "";
        break;
      }
      case UniformDataType::Color: {
        j["value"] = v4(1.0f);
        break;
      }
      case UniformDataType::Last: {
        break;
      }
    };

    if (type != UniformType::Texture) {
      _uniforms[(i32)type].push_back(Uniform(type, data_type, name, shader_type, j));
      return;
    }
    if (line_type.length() != std::string("texturen").length()) {
      log::error("Unknown uniform type \"{}\"", line);
      return;
    }
    i32 texture_index = line_type.substr(line_type.length() - 1, 1).at(0) - '0';
    if (texture_index < 0 || texture_index > MaxTextures) {
      log::error("Unknown uniform type \"{}\"", line);
      return;
    }
    _uniforms[(i32)type][texture_index] = Uniform(type, data_type, name, shader_type, j);
  }

  std::string Shader::Read(const std::filesystem::path &path, ShaderType shader_type) {
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
        // 9 is the length of "#include ", magic number
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
        // log::debug("Found include directive, path \"{}\", resolved \"{}\"", include, include_path.string());
        std::string include_data = Read(include_path, shader_type);
        if (include_data.empty()) {
          log::error("Shader include \"{}\" could not be read", include_path.string());
          buffer << "// include " << include << " could not be read\n";
          buffer << "// " << line << " END\n";
          continue;
        }

        buffer << include_data;
        buffer << "// " << line << " END\n";
        continue;
      } else if (line.find("#uniform ") == 0) {
        ParseUniform(line.substr(9), shader_type);
        buffer << "// " << line << '\n';
        continue;
      }

      buffer << line << '\n';
    }

    return buffer.str();
  }

  void Shader::SetUniformModel(const m4 &model) {
    for (Uniform &u : _uniforms[(i32)UniformType::Model])
      SetUniformM4(u.name, model);
  }

  void Shader::SetUniformView(const m4 &view) {
    for (Uniform &u : _uniforms[(i32)UniformType::View])
      SetUniformM4(u.name, view);
  }

  void Shader::SetUniformProjection(const m4 &projection) {
    for (Uniform &u : _uniforms[(i32)UniformType::Projection])
      SetUniformM4(u.name, projection);
  }

  void Shader::SetUniformTime(const f32 &time) {
    for (Uniform &u : _uniforms[(i32)UniformType::Time])
      SetUniformF32(u.name, time);
  }

  void Shader::SetUniformResolution(const v2 &resolution) {
    for (Uniform &u : _uniforms[(i32)UniformType::Resolution])
      SetUniformV2(u.name, resolution);
  }

  void Shader::SetUniformMouse(const v2 &mouse) {
    for (Uniform &u : _uniforms[(i32)UniformType::Mouse])
      SetUniformV2(u.name, mouse);
  }

  void Shader::SetUniformMouseDelta(const v2 &mouse_delta) {
    for (Uniform &u : _uniforms[(i32)UniformType::MouseDelta])
      SetUniformV2(u.name, mouse_delta);
  }

  void Shader::SetUniformTexture(u32 unit, u32 id) {
    if (unit >= MaxTextures) {
      log::error("Texture unit {} is out of range", unit);
      return;
    }

    const Uniform &uniform = _uniforms[(i32)UniformType::Texture][unit];
    if (uniform.name.empty()) {
      log::error("No uniform set for texture unit {}", unit);
      return;
    }

    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
    _uniform_texture_cache[unit] = id;
  }

  void Shader::SetOthers() {
    for (Uniform &u : _uniforms[(i32)UniformType::Other]) {
      switch (u.data_type) {
        case UniformDataType::Float: {
          f32 v = u.value["value"];
          SetUniformF32(u.name, v);
          break;
        }
        case UniformDataType::Int: {
          i32 v = u.value["value"];
          SetUniformI32(u.name, v);
          break;
        }
        case UniformDataType::Vec2: {
          v2 v = u.value["value"];
          SetUniformV2(u.name, v);
          break;
        }
        case UniformDataType::Vec3: {
          v3 v = u.value["value"];
          SetUniformV3(u.name, v);
          break;
        }
        case UniformDataType::Vec4: {
          v4 v = u.value["value"];
          SetUniformV4(u.name, v);
          break;
        }
        case UniformDataType::Mat4: {
          // j["value"] = m4(0.0f);
          break;
        }
        case UniformDataType::Sampler2D: {
          // j["value"] = "";
          break;
        }
        case UniformDataType::Color: {
          v4 v = u.value["value"];
          SetUniformV4(u.name, v);
          break;
        }
        case UniformDataType::Last: {
          break;
        }
      };

    }
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

    _uniform_locations.clear();

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

      std::istringstream stream(data);
      std::string line;
      std::stringstream buffer;
      i32 line_number = 1;
      while (std::getline(stream, line)) {
        buffer << std::setw(4) << std::setfill(' ') << line_number << ": " << line << '\n';
        ++line_number;
      }

      log::error("Shader data\n{}", buffer.str());

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
    _paths[(i32)type] = "";
    _watcher->removeWatch(_watch_ids[(i32)type]);

    log::debug("Unloaded shader, type {}", ShaderTypeToString(type));
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

    std::vector<Uniform> uniforms_copy[(i32)UniformType::Last];
    std::copy(_uniforms[(i32)type].begin(), _uniforms[(i32)type].end(), std::back_inserter(uniforms_copy[(i32)type]));

    for (i32 i = 0; i < (i32)UniformType::Last; ++i) {
      if ((UniformType)i == UniformType::Texture) {
        for (i32 j = 0; j < MaxTextures; ++j) {
          if (_uniforms[i][j].shader_type != type)
            continue;
          _uniforms[i][j].name = "";
          _uniforms[i][j].value = json::object();
          _uniforms[i][j].shader_type = ShaderType::Last;
        }
        continue;
      }

      _uniforms[i].erase(std::remove_if(_uniforms[i].begin(), _uniforms[i].end(), [type](const Uniform &u) {
        return u.shader_type == type;
      }), _uniforms[i].end());
    }

    std::string data = Read(_paths[(i32)type], type);
    if (data.empty())
      return false;

    u32 shader_id = CompileShader(type, data);
    if (shader_id == 0) {
      std::copy(uniforms_copy[(i32)type].begin(), uniforms_copy[(i32)type].end(), std::back_inserter(_uniforms[(i32)type]));
      return false;
    }

    if (_shaders[(i32)type] != 0)
      glDeleteShader(_shaders[(i32)type]);
    _shaders[(i32)type] = shader_id;

    return true;
  }

  i32 Shader::GetUniformLocation(const std::string &name) {
    if (_program == 0) {
      log::error("Shader program {} not compiled", _program);
      return -1;
    }

    if (_uniform_locations.count(name))
      return _uniform_locations[name];

    i32 location = glGetUniformLocation(_program, name.c_str());
    if (location == -1)
      log::error("Shader uniform \"{}\" not found", name);
    _uniform_locations[name] = location;
    return location;
  }

  void Shader::SetUniformV2(const std::string &name, const v2 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform2fv(location, 1, value_ptr(value));
  }

  void Shader::SetUniformV3(const std::string &name, const v3 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform3fv(location, 1, value_ptr(value));
  }

  void Shader::SetUniformV4(const std::string &name, const v4 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform4fv(location, 1, value_ptr(value));
  }

  void Shader::SetUniformM3(const std::string &name, const m3 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
  }

  void Shader::SetUniformM4(const std::string &name, const m4 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
  }

  void Shader::SetUniformF32(const std::string &name, const f32 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform1f(location, value);
  }

  void Shader::SetUniformI32(const std::string &name, const i32 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform1i(location, value);
  }

  void Shader::SetUniformU32(const std::string &name, const u32 &value) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform1ui(location, value);
  }

  void Shader::SetUniformF32V(const std::string &name, const f32 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform1fv(location, count, value);
  }

  void Shader::SetUniformI32V(const std::string &name, const i32 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform1iv(location, count, value);
  }

  void Shader::SetUniformU32V(const std::string &name, const u32 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform1uiv(location, count, value);
  }

  void Shader::SetUniformV2V(const std::string &name, const v2 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform2fv(location, count, value_ptr(value[0]));
  }

  void Shader::SetUniformV3V(const std::string &name, const v3 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform3fv(location, count, value_ptr(value[0]));
  }

  void Shader::SetUniformV4V(const std::string &name, const v4 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniform4fv(location, count, value_ptr(value[0]));
  }

  void Shader::SetUniformM3V(const std::string &name, const m3 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniformMatrix3fv(location, count, GL_FALSE, value_ptr(value[0]));
  }

  void Shader::SetUniformM4V(const std::string &name, const m4 *value, u32 count) {
    i32 location = GetUniformLocation(name);
    if (location == -1)
      return;
    glUniformMatrix4fv(location, count, GL_FALSE, value_ptr(value[0]));
  }

  void Shader::ShowData(Uniform &u) {
    switch (u.data_type) {
      case UniformDataType::Float: {
        json j = u.value;
        f32 v = j["value"];
        if (!axl::ShowData(u.name, v))
          return;
        SetUniformF32(u.name, v);
        j["value"] = v;
        u.value = j;
        break;
      }
      case UniformDataType::Int: {
        json j = u.value;
        i32 v = j["value"];
        if (!axl::ShowData(u.name, v))
          return;
        SetUniformI32(u.name, v);
        j["value"] = v;
        u.value = j;
        break;
      }
      case UniformDataType::Vec2: {
        json j = u.value;
        v2 v = j["value"];
        if (!axl::ShowData(u.name, v))
          return;
        SetUniformV2(u.name, v);
        j["value"] = v;
        u.value = j;
        break;
      }
      case UniformDataType::Vec3: {
        json j = u.value;
        v3 v = j["value"];
        if (!axl::ShowData(u.name, v))
          return;
        SetUniformV3(u.name, v);
        j["value"] = v;
        u.value = j;
        break;
      }
      case UniformDataType::Vec4: {
        json j = u.value;
        v4 v = j["value"];
        if (!axl::ShowData(u.name, v))
          return;
        SetUniformV4(u.name, v);
        j["value"] = v;
        u.value = j;
        break;
      }
      case UniformDataType::Mat4: {
        break;
      }
      case UniformDataType::Sampler2D: {
        break;
      }
      case UniformDataType::Color: {
        json j = u.value;
        v4 v = j["value"];
        if (!axl::ShowDataColor(u.name, v))
          return;
        SetUniformV4(u.name, v);
        j["value"] = v;
        u.value = j;
        break;
      }
      case UniformDataType::Last: {
        break;
      }
    };
  }

  bool Shader::ShowData() {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::CollapsingHeader("Shader", flags)) {
      for (Uniform &u : _uniforms[(i32)UniformType::Other]) {
        ShowData(u);
      }
      for (i32 i = 0; i < MaxTextures; ++i) {
        if (_uniform_texture_cache[i] == 0)
          continue;
        std::string texture_unit_name = "Texture" + std::to_string(i);
        const u32 &id = _uniform_texture_cache[i];
        ImGui::Text("%s : %s", _uniforms[(i32)UniformType::Texture][i].name.c_str(), texture_unit_name.c_str());
        f32 size = ImGui::CalcItemWidth();
        ImGui::Image((void *)(intptr_t)id, v2(size));
      }
    }
    return false;
  }

} // namespace axl
