#include <axolotl/shader.hh>

#include <fstream>
#include <sstream>
#include <glad.h>
#include <efsw/efsw.hpp>
#include <iostream>

namespace axl {

  Shader::Shader(u32 shader_id):
    shader_id(shader_id)
  {
    ShaderStore::GetData(shader_id).instances++;
  }

  Shader::Shader(const ShaderData &data) {
    ShaderStore::RegisterShader(*this, data);
  }

  Shader::Shader(const Shader &other) {
    ShaderStore::GetData(other.shader_id).instances++;
    shader_id = other.shader_id;
  }

  Shader::Shader(Shader &&other) {
    ShaderStore::GetData(other.shader_id).instances++;
    shader_id = other.shader_id;
  }

  Shader::~Shader() {
    log::debug("Shader destructor");
    ShaderStore::DeregisterShader(shader_id);
  }

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
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "vertex")
      return ShaderType::Vertex;
    if (lower == "fragment")
      return ShaderType::Fragment;
    if (lower == "geometry")
      return ShaderType::Geometry;
    if (lower == "compute")
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

  i32 Shader::GetUniformLocation(const std::string &name) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data.gl_id == 0) {
      log::error("Shader program {} not compiled", data.gl_id);
      return -1;
    }

    if (data._uniform_locations.count(name))
      return data._uniform_locations[name];

    i32 location = glGetUniformLocation(data.gl_id, name.c_str());
    if (location == -1)
      log::error("Shader uniform \"{}\" not found", name);
    log::warn("Retrieving uniform location at runtime. Program {}, Uniform {}", data.gl_id, name);
    data._uniform_locations[name] = location;
    return location;
  }

  u32 ShaderStore::GetShaderFromPath(const ShaderData &data) {
    for (auto itr = _shader_data.cbegin(); itr != _shader_data.cend(); ++itr) {
      bool equal = true;
      for (u32 i = 0; i < (i32)ShaderType::Last; ++i) {
        if (itr->second.paths[i] != data.paths[i]) {
          equal = false;
          break;
        }
      }
      if (equal)
        return itr->first;
    }
    return 0;
  }

  ShaderData & ShaderStore::GetData(u32 shader_id) {
    return _shader_data[shader_id];
  }

  void Shader::Bind() {
    ShaderData &data = ShaderStore::GetData(shader_id);
    AXL_ASSERT(data.gl_id, "Shader program {} not compiled", data.gl_id);
    glUseProgram(data.gl_id);
  }

  void Shader::Unbind() {
    glUseProgram(0);
  }

  void Shader::UnloadShader(ShaderType type) {
    ShaderStore::UnloadShader(*this, type);
  }

  void Shader::LoadFromPath(ShaderType type, const std::filesystem::path &path) {
    ShaderStore::LoadFromPath(*this, type, path);
  }

  void Shader::LoadFromData(ShaderType type, const std::string &data) {
    ShaderStore::LoadFromData(*this, type, data);
  }

  bool Shader::ReloadShader(ShaderType type) {
    return ShaderStore::ReloadShader(*this, type);
  }

  bool Shader::ReloadProgram() {
    return ShaderStore::ReloadProgram(*this);
  }

  bool Shader::Compile() {
    return ShaderStore::CompileProgram(*this);
  }

  bool Shader::Recompile() {
    return ShaderStore::RecompileProgram(*this);
  }

  void Shader::ShowComponent() {
    // TODO
  }

  std::unordered_map<u32, ShaderData> & ShaderStore::GetAllShadersData() {
    return _shader_data;
  }

  Shader::operator u32() const {
    return ShaderStore::GetRendererID(shader_id);
  }

  Shader ShaderStore::FromID(u32 id) {
    return Shader(id);
  }

  u32 ShaderStore::GetRendererID(u32 shader_id) {
    return _shader_data[shader_id].gl_id;
  }

  void ShaderStore::RegisterShader(Shader &shader, const ShaderData &data) {
    if (data.paths[(i32)ShaderType::Vertex].empty() || data.paths[(i32)ShaderType::Fragment].empty()) {
      log::error("Trying to register shader that has no vertex or fragment shader");
      return;
    }

    u32 shader_id = GetShaderFromPath(data);
    if (shader_id > 0) {
      shader.shader_id = shader_id;
      _shader_data[shader_id].instances++;
      return;
    }

    _id_counter++;
    shader.shader_id = _id_counter;
    _shader_data.insert(std::pair<u32, ShaderData>(shader.shader_id, data));
    _shader_data[shader.shader_id].instances++;
    _shader_queue.emplace(shader);

    _shader_data[shader.shader_id].id = shader.shader_id;

    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (data.paths[i].empty())
        continue;
      LoadFromPath(shader, (ShaderType)i, data.paths[i]);
    }
    CompileProgram(shader);
  }

  void ShaderStore::ProcessQueue() {
    while (!_shader_queue.empty()) {
      Shader &shader = _shader_queue.front();
      ShaderData &data = _shader_data[shader.shader_id];

      _shader_queue.pop();
    }
  }

  void ShaderStore::DeregisterShader(u32 shader_id) {
    if (!_shader_data.count(shader_id)) {
      log::error("Shader id {} not registered", shader_id);
      return;
    }

    _shader_data[shader_id].instances--;
    if (_shader_data[shader_id].instances > 0)
      return;

    log::debug("Deleting shader id {}", shader_id);

    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (_shader_data[shader_id].shaders[i] == 0)
        continue;
      glDeleteShader(_shader_data[shader_id].shaders[i]);
    }

    glDeleteProgram(_shader_data[shader_id].gl_id);
    _shader_data.erase(shader_id);
  }

  void ShaderStore::LoadFromPath(Shader &shader, ShaderType type, const std::filesystem::path &path) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data.shaders[(i32)type] != 0) {
      log::warn("{} shader already compiled", Shader::ShaderTypeToString(type));
      return;
    }

    log::debug("Loading {} shader from {}", Shader::ShaderTypeToString(type), path.string());
    std::string source = ReadShader(type, path);

    data.paths[(i32)type] = path;

    LoadFromData(shader, type, source);
  }

  void ShaderStore::LoadFromData(Shader &shader, ShaderType type, const std::string &source) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data.shaders[(i32)type] != 0) {
      log::warn("{} shader already compiled", Shader::ShaderTypeToString(type));
      return;
    }

    data.shaders[(i32)type] = CompileShader(type, source);
  }

  std::string ShaderStore::ReadShader(ShaderType type, const std::filesystem::path &path) {
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
        log::debug("Found include directive, path \"{}\", resolved \"{}\"", include, include_path.string());
        std::string include_data = ReadShader(type, include_path);
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

  u32 ShaderStore::CompileShader(ShaderType type, const std::string &source) {
    if (source.empty())
      return 0;

    u32 shader_id = glCreateShader(ShaderTypeToGL(type));

    const char *cstr = source.c_str();
    glShaderSource(shader_id, 1, &cstr, nullptr);
    glCompileShader(shader_id);

    i32 status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
      log::debug("Shader {} compiled, type {}", shader_id, Shader::ShaderTypeToString(type));
      return shader_id;
    }

    i32 length;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

    std::string log(length, ' ');
    glGetShaderInfoLog(shader_id, length, &length, &log[0]);

    log::error("Shader {} compilation failed, type: {}\n{}", shader_id, Shader::ShaderTypeToString(type), log);

    std::istringstream stream(source);
    std::string line;
    std::stringstream buffer;
    i32 line_number = 1;
    while (std::getline(stream, line)) {
      buffer << std::setw(4) << std::setfill(' ') << line_number << ": " << line << '\n';
      ++line_number;
    }

    log::error("Shader source\n{}", buffer.str());

    glDeleteShader(shader_id);
    return 0;
  }

  bool ShaderStore::CompileProgram(Shader &shader) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data.gl_id != 0) {
      log::warn("Shader id {} already compiled", shader.shader_id);
      return false;
    }

    data.gl_id = glCreateProgram();

    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (data.shaders[i] == 0)
        continue;

      glAttachShader(data.gl_id, data.shaders[i]);
    }

    glLinkProgram(data.gl_id);

    i32 status;
    glGetProgramiv(data.gl_id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
      i32 length;
      glGetProgramiv(data.gl_id, GL_INFO_LOG_LENGTH, &length);

      std::string log(length, ' ');
      glGetProgramInfoLog(data.gl_id, length, &length, &log[0]);

      log::error("Shader linking failed\n{}", log);
      return false;
    }

    log::debug("Shader id {} compiled", shader.shader_id);

    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (data.shaders[i] == 0)
        continue;

      glDetachShader(data.gl_id, data.shaders[i]);
    }

    data._uniform_locations.clear();
    data._uniform_locations_reverse.clear();
    data._attribute_locations.clear();
    data._attribute_locations_reverse.clear();
    data._uniform_data_types.clear();

    data._uniform_v2.clear();
    data._uniform_v3.clear();
    data._uniform_v4.clear();
    data._uniform_m3.clear();
    data._uniform_m4.clear();
    data._uniform_f32.clear();
    data._uniform_f64.clear();
    data._uniform_i32.clear();
    data._uniform_u32.clear();
    data._uniform_textures.clear();

    GetUniformData(shader);
    VerifyUniforms(shader);

    return true;
  }

  bool ShaderStore::RecompileProgram(Shader &shader) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data.gl_id == 0) {
      log::warn("Shader id {} not compiled", shader.shader_id);
      return false;
    }

    glDeleteProgram(data.gl_id);
    data.gl_id = 0;

    return CompileProgram(shader);
  }

  void ShaderStore::GetUniformData(Shader &shader) {
    ShaderData &data = _shader_data[shader.shader_id];

    shader.Bind();
    i32 num_active_attribs = 0;
    i32 num_active_uniforms = 0;

    glGetProgramInterfaceiv(data.gl_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &num_active_attribs);
    glGetProgramInterfaceiv(data.gl_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_active_uniforms);

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
      glGetProgramResourceiv(data.gl_id, GL_PROGRAM_INPUT, i, properties.size(), properties.data(), properties.size(), nullptr, values.data());
      name_buffer.resize(values[0]);
      glGetProgramResourceName(data.gl_id, GL_PROGRAM_INPUT, i, name_buffer.size(), nullptr, name_buffer.data());
      std::string name(name_buffer.data());
      log::debug("Attribute {}: {} | location {}", i, name, values[3]);

      data._attribute_locations[name] = values[3];
      data._attribute_locations_reverse[values[3]] = name;
    }

    for (i32 i = 0; i < num_active_uniforms; ++i) {
      glGetProgramResourceiv(data.gl_id, GL_UNIFORM, i, properties.size(), properties.data(), properties.size(), nullptr, values.data());
      name_buffer.resize(values[0]);
      glGetProgramResourceName(data.gl_id, GL_UNIFORM, i, name_buffer.size(), nullptr, name_buffer.data());
      std::string name(name_buffer.data());

      log::debug("Uniforms {}: {} | location {}", i, name, values[3]);

      UniformDataType data_type = Shader::GLToUniformDataType(values[1]);
      i32 location = values[3];
      data._uniform_locations.insert({ name, location });
      data._uniform_locations_reverse.insert({ location, name });
      data._uniform_data_types.insert({ location, data_type });

      switch (data_type) {
        case UniformDataType::Vector2:
          data._uniform_v2.insert({ location, v2(1.0f) });
          shader.SetUniformV2(location, data._uniform_v2[location]);
          break;
        case UniformDataType::Vector3:
          data._uniform_v3.insert({ location, v3(1.0f) });
          shader.SetUniformV3(location, data._uniform_v3[location]);
          break;
        case UniformDataType::Vector4:
          data._uniform_v4.insert({ location, v4(1.0f) });
          shader.SetUniformV4(location, data._uniform_v4[location]);
          break;
        case UniformDataType::Matrix3:
          data._uniform_m3.insert({ location, m3(1.0f) });
          shader.SetUniformM3(location, data._uniform_m3[location]);
          break;
        case UniformDataType::Matrix4:
          data._uniform_m4.insert({ location, m4(1.0f) });
          shader.SetUniformM4(location, data._uniform_m4[location]);
          break;
        case UniformDataType::Float:
          data._uniform_f32.insert({ location, 1.0f });
          shader.SetUniformF32(location, data._uniform_f32[location]);
          break;
        case UniformDataType::Double:
          data._uniform_f64.insert({ location, 1.0 });
          break;
        case UniformDataType::Int:
          data._uniform_i32.insert({ location, 0 });
          shader.SetUniformI32(location, data._uniform_i32[location]);
          break;
        case UniformDataType::UInt:
          data._uniform_u32.insert({ location, 0u });
          shader.SetUniformU32(location, data._uniform_u32[location]);
          break;
        case UniformDataType::Texture:
        case UniformDataType::TextureArray:
          std::array<i32, MAX_TEXTURE_UNITS> textures;
          std::fill(textures.begin(), textures.end(),-1);
          data._uniform_textures.insert({ location, textures });

          for (i32 i = 0; i < (i32)TextureType::Last; ++i)
            shader.SetUniformTexture((TextureType)i, -1);
          break;
        case UniformDataType::Last:
          break;
      }
    }
  }

  void ShaderStore::UnloadShader(Shader &shader, ShaderType type) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data.shaders[(i32)type] == 0) {
      log::warn("{} shader in program {} not loaded", Shader::ShaderTypeToString(type), shader.shader_id);
      return;
    }

    glDeleteShader(data.shaders[(i32)type]);
    data.shaders[(i32)type] = 0;
    data.paths[(i32)type] = "";

    log::debug("Unloaded {} shader from program {}", Shader::ShaderTypeToString(type), shader.shader_id);
  }

  bool ShaderStore::ReloadShader(Shader &shader, ShaderType type) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data.shaders[(i32)type] == 0) {
      log::warn("{} shader in program {} not loaded", Shader::ShaderTypeToString(type), shader.shader_id);
      return false;
    }

    if (data.paths[(i32)type].empty()) {
      log::warn("{} shader in program {} has no path", Shader::ShaderTypeToString(type), shader.shader_id);
      return false;
    }

    log::debug("Reloading {} shader from program {}", Shader::ShaderTypeToString(type), shader.shader_id);

    std::string source = ReadShader(type, data.paths[(i32)type]);
    if (source.empty()) {
      log::error("Failed to read {} shader from {}", Shader::ShaderTypeToString(type), data.paths[(i32)type].string());
      return false;
    }

    u32 shader_id = CompileShader(type, source);
    if (shader_id == 0)
      return false;

    if (data.shaders[(i32)type] != 0)
      glDeleteShader(data.shaders[(i32)type]);
    data.shaders[(i32)type] = shader_id;

    return true;
  }

  bool ShaderStore::ReloadProgram(Shader &shader) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data.gl_id == 0) {
      log::warn("Program {} not loaded", shader.shader_id);
      return false;
    }

    bool success = true;
    for (i32 i = 0; i < (i32)ShaderType::Last; ++i) {
      if (data.paths[i].empty())
        continue;
      if (!ReloadShader(shader, (ShaderType)i))
        success = false;
    }
    return success;
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
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::Vector2)
      return;
    glUniform2fv(location, 1, value_ptr(value));
    data._uniform_v2[location] = value;
  }

  void Shader::SetUniformV3(u32 location, const v3 &value) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::Vector3)
      return;
    glUniform3fv(location, 1, value_ptr(value));
    data._uniform_v3[location] = value;
  }

  void Shader::SetUniformV4(u32 location, const v4 &value) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::Vector4)
      return;
    glUniform4fv(location, 1, value_ptr(value));
    data._uniform_v4[location] = value;
  }

  void Shader::SetUniformM3(u32 location, const m3 &value) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::Matrix3)
      return;
    glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(value));
    data._uniform_m3[location] = value;
  }

  void Shader::SetUniformM4(u32 location, const m4 &value) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::Matrix4)
      return;
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
    data._uniform_m4[location] = value;
  }

  void Shader::SetUniformF32(u32 location, const f32 &value) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::Float)
      return;
    glUniform1f(location, value);
    data._uniform_f32[location] = value;
  }

  void Shader::SetUniformI32(u32 location, const i32 &value) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::Int)
      return;
    glUniform1i(location, value);
    data._uniform_i32[location] = value;
  }

  void Shader::SetUniformU32(u32 location, const u32 &value) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (data._uniform_data_types[location] != UniformDataType::UInt)
      return;
    glUniform1ui(location, value);
    data._uniform_u32[location] = value;
  }

  void Shader::SetUniformTexture(TextureType type, i32 unit) {
    ShaderData &data = ShaderStore::GetData(shader_id);
    if (type == TextureType::Last || !data._uniform_textures.count((i32)UniformLocation::Textures))
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

    data._uniform_textures[(i32)UniformLocation::Textures][(i32)type] = value;

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

  void ShaderStore::VerifyUniforms(Shader &shader) {
    ShaderData &data = _shader_data[shader.shader_id];
    if (data._uniform_data_types.count((i32)UniformLocation::ModelMatrix) &&
        data._uniform_data_types[(i32)UniformLocation::ModelMatrix] != UniformDataType::Matrix4)
        log::warn("Shader uniform 'ModelMatrix' is not a matrix4");
    if (data._uniform_data_types.count((i32)UniformLocation::ViewMatrix) &&
        data._uniform_data_types[(i32)UniformLocation::ViewMatrix] != UniformDataType::Matrix4)
        log::warn("Shader uniform 'ViewMatrix' is not a matrix4");
    if (data._uniform_data_types.count((i32)UniformLocation::ProjectionMatrix) &&
        data._uniform_data_types[(i32)UniformLocation::ProjectionMatrix] != UniformDataType::Matrix4)
        log::warn("Shader uniform 'ProjectionMatrix' is not a matrix4");
    if (data._uniform_data_types.count((i32)UniformLocation::Time) &&
        data._uniform_data_types[(i32)UniformLocation::Time] != UniformDataType::Float)
        log::warn("Shader uniform 'Time' is not a f32");
    if (data._uniform_data_types.count((i32)UniformLocation::Resolution) &&
        data._uniform_data_types[(i32)UniformLocation::Resolution] != UniformDataType::Vector2)
        log::warn("Shader uniform 'Resolution' is not a vector2");
    if (data._uniform_data_types.count((i32)UniformLocation::Mouse) &&
        data._uniform_data_types[(i32)UniformLocation::Mouse] != UniformDataType::Vector2)
        log::warn("Shader uniform 'Mouse' is not a vector2");

    if (data._uniform_data_types.count((i32)UniformLocation::Textures) &&
        data._uniform_data_types[(i32)UniformLocation::Textures] != UniformDataType::Texture &&
        data._uniform_data_types[(i32)UniformLocation::Textures] != UniformDataType::TextureArray)
    {
        log::warn("Shader uniform 'Textures' is not a texture or texture array");

        for (i32 i = 1; i < (i32)TextureType::Last; ++i)
          if (data._uniform_data_types.count((i32)UniformLocation::Textures + i))
            log::warn("Shader uniform at location {} collides with other texture uniforms", (i32)UniformLocation::Textures + i);
    }
  }
#pragma endregion

} // namespace axl
