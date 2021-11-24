#version 460 core

#include utils

#uniform other color color
uniform vec4 color;
#uniform other sampler2D diffuseTex
uniform sampler2D diffuseTex;

in Vertex {
  vec3 normals;
  vec2 texCoord;
} IN;

out vec4 fragColor;

void main () {
  fragColor = texture(diffuseTex, IN.texCoord) * color;
}
