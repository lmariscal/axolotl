file(GLOB AXOLOTL_EDITOR_SOURCES
  editor.cc
)

add_executable(axolotl_editor ${AXOLOTL_EDITOR_SOURCES})

set_target_properties(axolotl_editor PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
)

set_target_properties(axolotl_editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist/bin)
set_target_properties(axolotl_editor PROPERTIES OUTPUT_NAME axolotl)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(axolotl_editor PUBLIC AXOLOTL_DEBUG=1)
endif()

target_link_libraries(axolotl_editor PRIVATE axolotl)
