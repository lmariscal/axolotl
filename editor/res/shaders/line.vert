#version 460 core

#include utils

layout(location = ATTRIB_POSITION) in vec3 position;
layout(location = 1) in vec4 color;

layout(location = UNIFORM_MODEL_MATRIX) uniform mat4 model;
layout(location = UNIFORM_VIEW_MATRIX) uniform mat4 view;
layout(location = UNIFORM_PROJECTION_MATRIX) uniform mat4 projection;

layout(location = 0) out Vertex {
  vec4 color;
}
OUT;

void main() {
  mat4 mvp = projection * view * model;

  gl_Position = mvp * vec4(position, 1.0);
  // gl_Position.z = gl_Position.w;

  OUT.color = color;
}
