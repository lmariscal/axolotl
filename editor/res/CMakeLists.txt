file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res)

# Shaders

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res/shaders)

file(GLOB_RECURSE AXOLOTL_SHADERS_VERT LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/editor/res/shaders/*.vert")
file(GLOB_RECURSE AXOLOTL_SHADERS_FRAG LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/editor/res/shaders/*.frag")
file(GLOB_RECURSE AXOLOTL_SHADERS_GLSL LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/editor/res/shaders/*.glsl")

set(AXOLOTL_SHADERS ${AXOLOTL_SHADERS_VERT} ${AXOLOTL_SHADERS_FRAG} ${AXOLOTL_SHADERS_GLSL})

set(AXOLOTL_OUT_SHADERS)
foreach(SHADER_FILE ${AXOLOTL_SHADERS})
  get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
  set(AXOLOTL_OUT_SHADERS ${AXOLOTL_OUT_SHADERS} "${CMAKE_BINARY_DIR}/dist/res/shaders/${SHADER_NAME}")
endforeach()

add_custom_target(axolotl_shaders ALL DEPENDS ${AXOLOTL_OUT_SHADERS})

foreach(SHADER_FILE ${AXOLOTL_SHADERS})
  file(COPY "${SHADER_FILE}" DESTINATION "${CMAKE_BINARY_DIR}/dist/res/shaders")
  get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
  set(OUT_SHADER "${CMAKE_BINARY_DIR}/dist/res/shaders/${SHADER_NAME}")
  add_custom_command(
    OUTPUT ${OUT_SHADER}
    COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_FILE} ${OUT_SHADER}
    DEPENDS ${SHADER_FILE}
    VERBATIM
  )
endforeach()

add_dependencies(axolotl_editor axolotl_shaders)

# Textures

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res/textures)

file (GLOB_RECURSE AXOLOTL_TEXTURES LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/editor/res/textures/*")
add_custom_target(axolotl_textures ALL DEPENDS ${AXOLOTL_TEXTURES})

foreach(TEXTURE_FILE ${AXOLOTL_TEXTURES})
  get_filename_component(FILE_NAME ${TEXTURE_FILE} NAME)
  set(FILE_NAME "${CMAKE_BINARY_DIR}/dist/res/textures/${FILE_NAME}")
  configure_file(${TEXTURE_FILE} ${FILE_NAME} COPYONLY)
endforeach()

add_dependencies(axolotl_editor axolotl_textures)

# MISC

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/dist/res/misc)

file (GLOB_RECURSE AXOLOTL_RES LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/editor/res/misc/*")
add_custom_target(axolotl_res_misc ALL DEPENDS ${AXOLOTL_RES})

foreach(RES_FILE ${AXOLOTL_RES})
  get_filename_component(FILE_NAME ${RES_FILE} NAME)
  set(FILE_NAME "${CMAKE_BINARY_DIR}/dist/res/misc/${FILE_NAME}")
  configure_file(${RES_FILE} ${FILE_NAME} COPYONLY)
endforeach()

add_dependencies(axolotl_editor axolotl_res_misc)
