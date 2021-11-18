file(GLOB AXOLOTL_SOURCES
  src/axolotl.cc
  src/window.cc
  src/renderer.cc
  src/shader.cc
  src/mesh.cc
  src/texture.cc
  src/framebuffer.cc
  src/gui.cc
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

if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(axolotl PUBLIC AXOLOTL_DEBUG=1)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  target_compile_definitions(axolotl PUBLIC AXOLOTL_64=1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  target_compile_definitions(axolotl PUBLIC AXOLOTL_32=1)
endif()

target_link_libraries(axolotl PUBLIC glm)
target_link_libraries(axolotl PUBLIC spdlog)
target_link_libraries(axolotl PUBLIC imgui)

target_link_libraries(axolotl PRIVATE EnTT::EnTT)
target_link_libraries(axolotl PRIVATE glad)
target_link_libraries(axolotl PRIVATE glfw)
target_link_libraries(axolotl PRIVATE imguizmo)
target_link_libraries(axolotl PRIVATE ImTerm)
target_link_libraries(axolotl PRIVATE nlohmann_json::nlohmann_json) # json
target_link_libraries(axolotl PRIVATE tinygltf)
target_link_libraries(axolotl PRIVATE stb)
target_link_libraries(axolotl PRIVATE OpenGL::GL)

target_include_directories(axolotl PUBLIC ${CMAKE_SOURCE_DIR}/core/inc)
target_include_directories(axolotl PRIVATE ${OPENGL_INCLUDE_DIR})
