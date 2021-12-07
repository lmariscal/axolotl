file(GLOB_RECURSE AXOLOTL_SOURCES
  src/*.cc
  inc/**/*.h
)

set(PUBLIC_LIBS
  ergo
  glm
  spdlog
  imgui
  imguizmo
  imterm
  IconFontCppHeaders
  EnTT::EnTT
  nlohmann_json::nlohmann_json
  nativefiledialog
  stduuid
  assimp
)

set(PRIVATE_LIBS
  efsw
  glad
  glfw
  stb
  OpenGL::GL
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

foreach(LIB ${PUBLIC_LIBS})
  target_link_libraries(axolotl PUBLIC ${LIB})
endforeach()

foreach(LIB ${PRIVATE_LIBS})
  target_link_libraries(axolotl PRIVATE ${LIB})
endforeach()

target_include_directories(axolotl PUBLIC ${CMAKE_SOURCE_DIR}/core/inc)
target_include_directories(axolotl PUBLIC ${CMAKE_SOURCE_DIR}/third)
# target_include_directories(axolotl PUBLIC ${CMAKE_SOURCE_DIR}/third/stduuid/include)
target_include_directories(axolotl PRIVATE ${OPENGL_INCLUDE_DIR})

# RUNTIME

add_executable(axolotl_runtime ${AXOLOTL_SOURCES})

set_target_properties(axolotl_runtime PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

set_target_properties(axolotl_runtime PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/bin)
set_target_properties(axolotl_runtime PROPERTIES OUTPUT_NAME axolotl)

foreach(LIB ${PUBLIC_LIBS})
  target_link_libraries(axolotl_runtime PUBLIC ${LIB})
endforeach()

foreach(LIB ${PRIVATE_LIBS})
  target_link_libraries(axolotl_runtime PRIVATE ${LIB})
endforeach()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(axolotl_runtime PUBLIC AXOLOTL_DEBUG=1)
endif()
target_compile_definitions(axolotl_runtime PUBLIC AXOLOTL_RUNTIME=1)

target_include_directories(axolotl_runtime PUBLIC ${CMAKE_SOURCE_DIR}/core/inc)
target_include_directories(axolotl_runtime PUBLIC ${CMAKE_SOURCE_DIR}/third)
target_include_directories(axolotl_runtime PRIVATE ${OPENGL_INCLUDE_DIR})
