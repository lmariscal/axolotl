# Ergo

add_subdirectory(ergo)

# ASSIMP

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
else()
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endif()
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)

# EFSW

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
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
add_subdirectory(glm)

# IconFontCppHeaders

add_subdirectory(IconFontCppHeaders)

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

project(imterm)
add_library(imterm INTERFACE)

target_include_directories(imterm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/ImTerm/include)

# Native File Manager

file(GLOB NFM_SOURCES nativefiledialog/src/nfd_common.c)

if (WIN32)
  list(APPEND NFM_SOURCES nativefiledialog/src/nfd_win.cpp)
  list(APPEND PREPROCESSOR_DEFINITIONS
      UNICODE
      _UNICODE
  )
elseif (APPLE)
  list(APPEND NFM_SOURCES nativefiledialog/src/nfd_cocoa.m)
elseif (LINUX)
  list(APPEND NFM_SOURCES nativefiledialog/src/nfd_gtk.c)
else()
  message(FATAL_ERROR "Cannot detect your system")
endif()

add_library(nativefiledialog ${NFM_SOURCES})
target_include_directories(nativefiledialog PUBLIC nativefiledialog/src/include)
target_compile_definitions(nativefiledialog PUBLIC ${PREPROCESSOR_DEFINITIONS})

if(LINUX)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(GTK "gtk+-3.0")
    if (GTK_FOUND)
      target_link_libraries(nativefiledialog ${GTK_LIBRARIES})
      add_definitions(${GTK_CFLAGS} ${GTK_CFLAGS_OTHER})
    endif()
  endif()
endif()

# JSON

add_subdirectory(json)

# SPDLog

set(SPDLOG_MASTER_PROJECT OFF CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

# STB

project(stb)
add_library(stb INTERFACE)

target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb)

# stduuid

set(UUID_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(stduuid)
