#version 460 core

#include utils

#uniform other color color
uniform vec4 color;
#uniform other sampler2D diffuseTex
uniform sampler2D diffuseTex;

in Vertex {
  vec2 texCoord;
} IN;

out vec4 fragColor;

void main () {
  fragColor = texture(diffuseTex, IN.texCoord) * color;
  // vec4 color = vec4(rgb(255.0f, 137.0f, 123.0f), 1.0f);
  // fragColor = color;
}
