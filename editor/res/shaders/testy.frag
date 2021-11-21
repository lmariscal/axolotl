#version 460 core

#include utils

// uniform sampler2D diffuseTex;

// in Vertex {
//   vec2 texCoord;
//   vec4 color;
// } IN;

out vec4 fragColor;

void main () {
  // fragColor = texture(diffuseTex, IN.texCoord);
  vec3 color = rgb(255.0f, 137.0f, 123.0f);
  fragColor = vec4(color, 1.0f);
}
