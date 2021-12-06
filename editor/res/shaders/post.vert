#version 460 core

#include utils

layout(location = ATTRIB_POSITION) in vec3 position;
layout(location = ATTRIB_NORMAL) in vec3 normal;
layout(location = ATTRIB_TEXCOORD) in vec2 tex_coord;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

layout(location = 0) out Vertex {
  vec2 tex_coord;
} OUT;

void main() {
  mat4 mvp = projection * view * model;
  gl_Position = mvp * vec4(position, 1.0);
  OUT.tex_coord = tex_coord;
}
