#version 460 core

#include utils

layout(location = UNIFORM_SKYBOX) uniform samplerCube skybox;

layout(location = 0) in Vertex {
  vec3 tex_coord;
} IN;

layout(location = 0) out vec4 frag_color;

void main () {
  frag_color = texture(skybox, IN.tex_coord);
}
