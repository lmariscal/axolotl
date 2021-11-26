#version 460 core

#include utils

layout (location = UNIFORM_TEXTURES) uniform sampler2D textures[TEXTURE_COUNT];
#uniform other color color
uniform vec4 color;

layout (location = 0) in Vertex {
  vec3 normal;
  vec2 tex_coord;
} IN;

layout (location = 0) out vec4 frag_color;

void main () {
  vec4 normal = texture(textures[TEXTURE_NORMALS], IN.tex_coord);
  frag_color = texture(textures[TEXTURE_DIFFUSE], IN.tex_coord) * color * normal;
}
