#version 460 core

#include utils

layout (location = UNIFORM_TEXTURES) uniform sampler2D textures[TEXTURE_COUNT];

layout (location = 0) in Vertex {
  vec3 normal;
  vec2 tex_coord;
} IN;

layout (location = 0) out vec4 frag_color;

void main () {
  frag_color = texture(textures[TEXTURE_DIFFUSE], IN.tex_coord);
}
