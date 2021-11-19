#EFSW

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(efsw)

# EnTT

add_subdirectory(entt)

# GLAD

add_subdirectory(glad)

# GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# GLM

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(glm)

# ImGUI

project(imgui)
file(GLOB IMGUI_SOURCES
  imgui/imgui.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui_demo.cpp

  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui ${IMGUI_SOURCES})
target_link_libraries(imgui PRIVATE glfw)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)

# ImGuizmo

project(imguizmo)
file(GLOB IMGUIZMO_SOURCES
  ImGuizmo/GraphEditor.cpp
  ImGuizmo/ImCurveEdit.cpp
  ImGuizmo/ImGradient.cpp
  ImGuizmo/ImGuizmo.cpp
  ImGuizmo/ImSequencer.cpp
)
add_library(imguizmo ${IMGUIZMO_SOURCES})
target_link_libraries(imguizmo PRIVATE imgui)
target_include_directories(imguizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo)

# ImTerm

add_subdirectory(ImTerm)

# JSON

add_subdirectory(json)

# SPDLog

set(SPDLOG_MASTER_PROJECT OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

# STB

project(stb)
add_library(stb INTERFACE)

target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)

# TinyGLTF

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(tinygltf)
