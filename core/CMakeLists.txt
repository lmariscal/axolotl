file(GLOB AXOLOTL_SOURCES
  src/axolotl.cc
  src/window.cc
  src/renderer.cc
  src/shader.cc
  src/mesh.cc
  src/texture.cc
  src/framebuffer.cc
  src/gui.cc
  src/scene.cc
  src/iomanager.cc
  src/terminal.cc
  src/camera.cc
  src/transform.cc
  src/ento.cc
  src/component.cc
  src/model.cc
  src/material.cc
)

if(NOT DEFINED OpenGL_GL_PREFERENCE)
  set(OpenGL_GL_PREFERENCE "GLVND")
endif()

find_package(OpenGL REQUIRED)

add_library(axolotl ${AXOLOTL_SOURCES})

set_target_properties(axolotl PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

set_target_properties(axolotl PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/lib)
set_target_properties(axolotl PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/lib)
set_target_properties(axolotl PROPERTIES OUTPUT_NAME axolotl)

target_compile_definitions(axolotl PUBLIC ENTT_ID_TYPE=std::uint64_t)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(axolotl PUBLIC AXOLOTL_DEBUG=1)
else()
  target_compile_definitions(axolotl PUBLIC ENTT_DISABLE_ASSERT=1)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions(axolotl PUBLIC AXOLOTL_64=1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  target_compile_definitions(axolotl PUBLIC AXOLOTL_32=1)
endif()

target_link_libraries(axolotl PUBLIC glm)
target_link_libraries(axolotl PUBLIC spdlog)
target_link_libraries(axolotl PUBLIC imgui)
target_link_libraries(axolotl PUBLIC imguizmo)
target_link_libraries(axolotl PUBLIC imterm)
target_link_libraries(axolotl PUBLIC IconFontCppHeaders)
target_link_libraries(axolotl PUBLIC EnTT::EnTT)
target_link_libraries(axolotl PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(axolotl PUBLIC stduuid)

target_link_libraries(axolotl PRIVATE efsw)
target_link_libraries(axolotl PRIVATE glad)
target_link_libraries(axolotl PRIVATE glfw)
target_link_libraries(axolotl PRIVATE assimp)
target_link_libraries(axolotl PRIVATE stb)
target_link_libraries(axolotl PRIVATE OpenGL::GL)

target_include_directories(axolotl PUBLIC ${CMAKE_SOURCE_DIR}/core/inc)
target_include_directories(axolotl PUBLIC ${CMAKE_SOURCE_DIR}/third)
# target_include_directories(axolotl PUBLIC ${CMAKE_SOURCE_DIR}/third/stduuid/include)
target_include_directories(axolotl PRIVATE ${OPENGL_INCLUDE_DIR})
