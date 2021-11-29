#include <axolotl/shader.h>

#include <fstream>
#include <sstream>
#include <glad.h>
#include <efsw/efsw.hpp>
#include <iostream>

namespace axl {

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

  void Shader::Init() {
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      _shaders[i] = 0;
      _watch_ids[i] = 0;
      _need_reload[i] = false;
    }
    _watcher = new efsw::FileWatcher();
    _shader_watcher = new ShaderWatcher(this);

    // _watcher->watch();

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

      if (_watcher)
        _watcher->removeWatch(_watch_ids[i]);
    }

    glDeleteProgram(_program);

    delete _shader_watcher;
    delete _watcher;

    _shaders_programs.erase(std::find(_shaders_programs.begin(), _shaders_programs.end(), this));
    log::debug("Shader {} destroyed", _program);
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
      }

      buffer << line << '\n';
    }

    return buffer.str();
  }

  void Shader::GetUniformData() {
    Bind();
    i32 num_active_attribs = 0;
    i32 num_active_uniforms = 0;

    glGetProgramInterfaceiv(_program, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &num_active_attribs);
    glGetProgramInterfaceiv(_program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_active_uniforms);

    log::debug("Active attributes: {}", num_active_attribs);
    log::debug("Active uniforms: {}", num_active_uniforms);

    std::vector<char> name_buffer(256);
    std::vector<u32> properties;
    properties.push_back(GL_NAME_LENGTH);
    properties.push_back(GL_TYPE);
    properties.push_back(GL_ARRAY_SIZE);
    properties.push_back(GL_LOCATION);
    std::vector<i32> values(properties.size());
    for (i32 i = 0; i < num_active_attribs; ++i) {
      glGetProgramResourceiv(_program, GL_PROGRAM_INPUT, i, properties.size(), properties.data(), properties.size(), nullptr, values.data());
      name_buffer.resize(values[0]);
      glGetProgramResourceName(_program, GL_PROGRAM_INPUT, i, name_buffer.size(), nullptr, name_buffer.data());
      std::string name(name_buffer.data());
      log::debug("Attribute {}: {} | location {}", i, name, values[3]);
    }

    for (i32 i = 0; i < num_active_uniforms; ++i) {
      glGetProgramResourceiv(_program, GL_UNIFORM, i, properties.size(), properties.data(), properties.size(), nullptr, values.data());
      name_buffer.resize(values[0]);
      glGetProgramResourceName(_program, GL_UNIFORM, i, name_buffer.size(), nullptr, name_buffer.data());
      std::string name(name_buffer.data());

      log::debug("Uniforms {}: {} | location {}", i, name, values[3]);

      UniformDataType data_type = GLToUniformDataType(values[1]);
      i32 location = values[3];
      _uniform_locations.insert({ name, location });
      _uniform_locations_reverse.insert({ location, name });
      _uniform_data_type.insert({ location, data_type });

      switch (data_type) {
        case UniformDataType::Vector2:
          _uniform_v2.insert({ location, v2(1.0f) });
          SetUniformV2(location, _uniform_v2[location]);
          break;
        case UniformDataType::Vector3:
          _uniform_v3.insert({ location, v3(1.0f) });
          SetUniformV3(location, _uniform_v3[location]);
          break;
        case UniformDataType::Vector4:
          _uniform_v4.insert({ location, v4(1.0f) });
          SetUniformV4(location, _uniform_v4[location]);
          break;
        case UniformDataType::Matrix3:
          _uniform_m3.insert({ location, m3(1.0f) });
          SetUniformM3(location, _uniform_m3[location]);
          break;
        case UniformDataType::Matrix4:
          _uniform_m4.insert({ location, m4(1.0f) });
          SetUniformM4(location, _uniform_m4[location]);
          break;
        case UniformDataType::Float:
          _uniform_f32.insert({ location, 1.0f });
          SetUniformF32(location, _uniform_f32[location]);
          break;
        case UniformDataType::Double:
          _uniform_f64.insert({ location, 1.0 });
          break;
        case UniformDataType::Int:
          _uniform_i32.insert({ location, 0 });
          SetUniformI32(location, _uniform_i32[location]);
          break;
        case UniformDataType::UInt:
          _uniform_u32.insert({ location, 0u });
          SetUniformU32(location, _uniform_u32[location]);
          break;
        case UniformDataType::Texture:
        case UniformDataType::TextureArray:
          std::array<i32, MaxTextures> textures;
          std::fill(textures.begin(), textures.end(),-1);
          _uniform_textures.insert({ location, textures });

          for (i32 i = 0; i < (i32)TextureType::Last; ++i)
            SetUniformTexture((TextureType)i, -1);
          break;
        case UniformDataType::Last:
          break;
      }
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
    _uniform_locations_reverse.clear();
    _uniform_data_type.clear();

    _uniform_v2.clear();
    _uniform_v3.clear();
    _uniform_v4.clear();
    _uniform_m3.clear();
    _uniform_m4.clear();
    _uniform_f32.clear();
    _uniform_f64.clear();
    _uniform_i32.clear();
    _uniform_u32.clear();
    _uniform_textures.clear();

    GetUniformData();
    VerifyUniforms();

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

    std::string data = Read(_paths[(i32)type], type);
    if (data.empty())
      return false;

    u32 shader_id = CompileShader(type, data);
    if (shader_id == 0) {
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

  bool Shader::ShowData() {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::CollapsingHeader("Shader", flags)) {
      for (auto itr = _uniform_data_type.cbegin(); itr != _uniform_data_type.cend(); ++itr) {
        const UniformDataType &type = itr->second;
        const i32 &location = itr->first;
        const std::string &name = _uniform_locations_reverse[location];

        switch (type) {
          case UniformDataType::Vector2:
            if (axl::ShowData(name, _uniform_v2[location]))
              SetUniformV2(location, _uniform_v2[location]);
            break;
          case UniformDataType::Vector3:
            if (axl::ShowData(name, _uniform_v3[location]))
              SetUniformV3(location, _uniform_v3[location]);
            break;
          case UniformDataType::Vector4:
            if (axl::ShowDataColor(name, _uniform_v4[location]))
              SetUniformV4(location, _uniform_v4[location]);
            break;
          case UniformDataType::Matrix3:
            break;
          case UniformDataType::Matrix4:
            break;
          case UniformDataType::Float:
            if (axl::ShowData(name, _uniform_f32[location]))
              SetUniformF32(location, _uniform_f32[location]);
            break;
          case UniformDataType::Double:
            break;
          case UniformDataType::Int:
            if (axl::ShowData(name, _uniform_i32[location]))
              SetUniformI32(location, _uniform_i32[location]);
            break;
          case UniformDataType::UInt:
            if (axl::ShowData(name, _uniform_u32[location]))
              SetUniformU32(location, _uniform_u32[location]);
            break;
          case UniformDataType::Texture:
          case UniformDataType::TextureArray:
            if (ImGui::CollapsingHeader(name.c_str())) {
              for (i32 i = 0; i < MaxTextures; ++i) {
                if (_uniform_textures[location][i] == -1)
                  continue;
                std::string type_name = Texture::TextureTypeToString((TextureType)i);
                axl::ShowDataTexture(type_name.c_str(), _uniform_textures[location][i]);
              }
            }
            break;
          case UniformDataType::Last:
            break;
        }
      }
    }
    return false;
  }

#pragma region Uniforms
  UniformDataType Shader::GLToUniformDataType(u32 type) {
    switch (type) {
      case GL_FLOAT_VEC2:
        return UniformDataType::Vector2;
      case GL_FLOAT_VEC3:
        return UniformDataType::Vector3;
      case GL_FLOAT_VEC4:
        return UniformDataType::Vector4;
      case GL_FLOAT_MAT3:
        return UniformDataType::Matrix3;
      case GL_FLOAT_MAT4:
        return UniformDataType::Matrix4;
      case GL_FLOAT:
        return UniformDataType::Float;
      case GL_DOUBLE:
        return UniformDataType::Double;
      case GL_INT:
        return UniformDataType::Int;
      case GL_UNSIGNED_INT:
        return UniformDataType::UInt;
      case GL_SAMPLER_2D:
        return UniformDataType::Texture;
      case GL_SAMPLER_2D_ARRAY:
        return UniformDataType::TextureArray;
    }
    return UniformDataType::Last;
  }

  u32 Shader::UniformDataTypeToGL(UniformDataType type) {
    switch (type) {
      case UniformDataType::Vector2:
        return GL_FLOAT_VEC2;
      case UniformDataType::Vector3:
        return GL_FLOAT_VEC3;
      case UniformDataType::Vector4:
        return GL_FLOAT_VEC4;
      case UniformDataType::Matrix3:
        return GL_FLOAT_MAT3;
      case UniformDataType::Matrix4:
        return GL_FLOAT_MAT4;
      case UniformDataType::Float:
        return GL_FLOAT;
      case UniformDataType::Double:
        return GL_DOUBLE;
      case UniformDataType::Int:
        return GL_INT;
      case UniformDataType::UInt:
        return GL_UNSIGNED_INT;
      case UniformDataType::Texture:
        return GL_SAMPLER_2D;
      case UniformDataType::TextureArray:
        return GL_SAMPLER_2D_ARRAY;
      case UniformDataType::Last:
        return 0;
    }
  }

  void Shader::SetUniformV2(u32 location, const v2 &value) {
    if (_uniform_data_type[location] != UniformDataType::Vector2)
      return;
    glUniform2fv(location, 1, value_ptr(value));
    _uniform_v2[location] = value;
  }

  void Shader::SetUniformV3(u32 location, const v3 &value) {
    if (_uniform_data_type[location] != UniformDataType::Vector3)
      return;
    glUniform3fv(location, 1, value_ptr(value));
    _uniform_v3[location] = value;
  }

  void Shader::SetUniformV4(u32 location, const v4 &value) {
    if (_uniform_data_type[location] != UniformDataType::Vector4)
      return;
    glUniform4fv(location, 1, value_ptr(value));
    _uniform_v4[location] = value;
  }

  void Shader::SetUniformM3(u32 location, const m3 &value) {
    if (_uniform_data_type[location] != UniformDataType::Matrix3)
      return;
    glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
    _uniform_m3[location] = value;
  }

  void Shader::SetUniformM4(u32 location, const m4 &value) {
    if (_uniform_data_type[location] != UniformDataType::Matrix4)
      return;
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
    _uniform_m4[location] = value;
  }

  void Shader::SetUniformF32(u32 location, const f32 &value) {
    if (_uniform_data_type[location] != UniformDataType::Float)
      return;
    glUniform1f(location, value);
    _uniform_f32[location] = value;
  }

  void Shader::SetUniformI32(u32 location, const i32 &value) {
    if (_uniform_data_type[location] != UniformDataType::Int)
      return;
    glUniform1i(location, value);
    _uniform_i32[location] = value;
  }

  void Shader::SetUniformU32(u32 location, const u32 &value) {
    if (_uniform_data_type[location] != UniformDataType::UInt)
      return;
    glUniform1ui(location, value);
    _uniform_u32[location] = value;
  }

  void Shader::SetUniformTexture(TextureType type, i32 unit) {
    if (type == TextureType::Last || !_uniform_textures.count((i32)UniformLocation::Textures))
      return;
    if (unit < 0)
      return;

    i32 location = (i32)UniformLocation::Textures + (i32)type;
    glUniform1i(location, unit);

    i32 active_texture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);

    glActiveTexture(GL_TEXTURE0 + unit);
    i32 value;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &value);

    _uniform_textures[(i32)UniformLocation::Textures][(i32)type] = value;

    glActiveTexture(active_texture);
  }

  void Shader::SetUniformV2(const std::string &name, const v2 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformV2(location, value);
  }

  void Shader::SetUniformV3(const std::string &name, const v3 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformV3(location, value);
  }

  void Shader::SetUniformV4(const std::string &name, const v4 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformV4(location, value);
  }

  void Shader::SetUniformM3(const std::string &name, const m3 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformM3(location, value);
  }

  void Shader::SetUniformM4(const std::string &name, const m4 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformM4(location, value);
  }

  void Shader::SetUniformF32(const std::string &name, const f32 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformF32(location, value);
  }

  void Shader::SetUniformI32(const std::string &name, const i32 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformI32(location, value);
  }

  void Shader::SetUniformU32(const std::string &name, const u32 &value) {
    u32 location = GetUniformLocation(name);
    SetUniformU32(location, value);
  }

  void Shader::VerifyUniforms() {
    if (_uniform_data_type.count((i32)UniformLocation::ModelMatrix) &&
        _uniform_data_type[(i32)UniformLocation::ModelMatrix] != UniformDataType::Matrix4)
        log::warn("Shader uniform 'ModelMatrix' is not a matrix4");
    if (_uniform_data_type.count((i32)UniformLocation::ViewMatrix) &&
        _uniform_data_type[(i32)UniformLocation::ViewMatrix] != UniformDataType::Matrix4)
        log::warn("Shader uniform 'ViewMatrix' is not a matrix4");
    if (_uniform_data_type.count((i32)UniformLocation::ProjectionMatrix) &&
        _uniform_data_type[(i32)UniformLocation::ProjectionMatrix] != UniformDataType::Matrix4)
        log::warn("Shader uniform 'ProjectionMatrix' is not a matrix4");
    if (_uniform_data_type.count((i32)UniformLocation::Time) &&
        _uniform_data_type[(i32)UniformLocation::Time] != UniformDataType::Float)
        log::warn("Shader uniform 'Time' is not a f32");
    if (_uniform_data_type.count((i32)UniformLocation::Resolution) &&
        _uniform_data_type[(i32)UniformLocation::Resolution] != UniformDataType::Vector2)
        log::warn("Shader uniform 'Resolution' is not a vector2");
    if (_uniform_data_type.count((i32)UniformLocation::Mouse) &&
        _uniform_data_type[(i32)UniformLocation::Mouse] != UniformDataType::Vector2)
        log::warn("Shader uniform 'Mouse' is not a vector2");

    if (_uniform_data_type.count((i32)UniformLocation::Textures) &&
        _uniform_data_type[(i32)UniformLocation::Textures] != UniformDataType::Texture &&
        _uniform_data_type[(i32)UniformLocation::Textures] != UniformDataType::TextureArray)
    {
        log::warn("Shader uniform 'Textures' is not a texture or texture array");

        for (i32 i = 1; i < (i32)TextureType::Last; ++i)
          if (_uniform_data_type.count((i32)UniformLocation::Textures + i))
            log::warn("Shader uniform at location {} collides with other texture uniforms", (i32)UniformLocation::Textures + i);
    }
  }
#pragma endregion

} // namespace axl
